/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SmartChargingManager.h"
#include "Connectors.h"
#include "GenericMessageSender.h"
#include "IChargePointConfig.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"
#include "Logger.h"
#include "WorkerThreadPool.h"

#include <algorithm>

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
SmartChargingManager::SmartChargingManager(const ocpp::config::IChargePointConfig&         stack_config,
                                           ocpp::config::IOcppConfig&                      ocpp_config,
                                           ocpp::database::Database&                       database,
                                           IChargePointEventsHandler&                      events_handler,
                                           ocpp::helpers::ITimerPool&                      timer_pool,
                                           ocpp::helpers::WorkerThreadPool&                worker_pool,
                                           Connectors&                                     connectors,
                                           const ocpp::messages::GenericMessagesConverter& messages_converter,
                                           ocpp::messages::IMessageDispatcher&             msg_dispatcher)
    : GenericMessageHandler<ClearChargingProfileReq, ClearChargingProfileConf>(CLEAR_CHARGING_PROFILE_ACTION, messages_converter),
      GenericMessageHandler<SetChargingProfileReq, SetChargingProfileConf>(SET_CHARGING_PROFILE_ACTION, messages_converter),
      GenericMessageHandler<GetCompositeScheduleReq, GetCompositeScheduleConf>(GET_COMPOSITE_SCHEDULE_ACTION, messages_converter),
      m_stack_config(stack_config),
      m_ocpp_config(ocpp_config),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_connectors(connectors),
      m_profile_db(ocpp_config, database),
      m_mutex(),
      m_cleanup_timer(timer_pool, "Profile cleanup")
{
    msg_dispatcher.registerHandler(CLEAR_CHARGING_PROFILE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<ClearChargingProfileReq, ClearChargingProfileConf>*>(this));
    msg_dispatcher.registerHandler(SET_CHARGING_PROFILE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<SetChargingProfileReq, SetChargingProfileConf>*>(this));
    msg_dispatcher.registerHandler(GET_COMPOSITE_SCHEDULE_ACTION,
                                   *dynamic_cast<GenericMessageHandler<GetCompositeScheduleReq, GetCompositeScheduleConf>*>(this));

    // Periodic timer to cleanup profiles
    m_cleanup_timer.setCallback([this] { this->m_worker_pool.run<void>(std::bind(&SmartChargingManager::cleanupProfiles, this)); });
    m_cleanup_timer.start(std::chrono::minutes(1u));
    cleanupProfiles();
}

/** @brief Destructor */
SmartChargingManager::~SmartChargingManager() { }

/** @copydoc bool ISmartChargingManager::getSetpoint(unsigned int,
                                                     ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>&,
                                                     ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>&,
                                                     ocpp::types::ocpp16::ChargingRateUnitType) */
bool SmartChargingManager::getSetpoint(unsigned int                                                       connector_id,
                                       ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& charge_point_setpoint,
                                       ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& connector_setpoint,
                                       ocpp::types::ocpp16::ChargingRateUnitType                          unit)
{
    bool ret = false;

    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    // Check connector
    Connector* connector = m_connectors.getConnector(connector_id);
    if (connector)
    {
        // Compute charge point setpoint
        charge_point_setpoint.clear();
        for (const auto& profile : m_profile_db.chargePointMaxProfiles())
        {
            // Check if the profile is active
            size_t period = 0;
            if (isProfileActive(connector, profile.second, period, DateTime::now()))
            {
                // Apply setpoint
                fillSetpoint(charge_point_setpoint, unit, profile.second, profile.second.chargingSchedule.chargingSchedulePeriod[period]);
                break;
            }
        }

        // Compute connector setpoint
        connector_setpoint.clear();
        computeSetpoint(connector, connector_setpoint, unit, m_profile_db.txProfiles());
        if (!connector_setpoint.isSet())
        {
            computeSetpoint(connector, connector_setpoint, unit, m_profile_db.txDefaultProfiles());
        }

        // Connector setpoint cannot be greater than charge point setpoint
        if (charge_point_setpoint.isSet())
        {
            if (!connector_setpoint.isSet() || (connector_setpoint.value().value > charge_point_setpoint.value().value))
            {
                // Connector setpoint becomes charge point setpoint
                connector_setpoint = charge_point_setpoint;
            }
        }

        ret = true;
    }

    return ret;
}

/** @copydoc bool ISmartChargingManager::installTxProfile(unsigned int, const ocpp::types::ocpp16::ChargingProfile&) */
bool SmartChargingManager::installTxProfile(unsigned int connector_id, const ocpp::types::ocpp16::ChargingProfile& profile)
{
    bool ret = false;

    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_DEBUG << "Install TxProfile on connector " << connector_id;

    // Check profile purpose and transaction id
    if ((profile.chargingProfilePurpose == ChargingProfilePurposeType::TxProfile) && !profile.transactionId.isSet())
    {
        // Install profile
        ret = m_profile_db.install(connector_id, profile);
    }

    return ret;
}

/** @copydoc void ISmartChargingManager::assignPendingTxProfiles(unsigned int), unsigned int) */
void SmartChargingManager::assignPendingTxProfiles(unsigned int connector_id, int transaction_id)
{
    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_DEBUG << "Assign pending TxProfile on connector " << connector_id << " for transaction " << transaction_id;

    // Assign profile
    m_profile_db.assignPendingTxProfiles(connector_id, transaction_id);
}

/** @copydoc void ISmartChargingManager::clearTxProfiles(unsigned int) */
void SmartChargingManager::clearTxProfiles(unsigned int connector_id)
{
    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG_DEBUG << "Clear TxProfile on connector " << connector_id;

    // Clear Tx profiles
    m_profile_db.clear(Optional<int>(), connector_id, ChargingProfilePurposeType::TxProfile);
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::ocpp16::ClearChargingProfileReq& request,
                                         ocpp::messages::ocpp16::ClearChargingProfileConf&      response,
                                         std::string&                                           error_code,
                                         std::string&                                           error_message)
{
    (void)error_code;
    (void)error_message;

    LOG_INFO << "Clear charging profile requested : id = " << (request.id.isSet() ? std::to_string(request.id) : "not set")
             << " - connectorId = " << (request.connectorId.isSet() ? std::to_string(request.connectorId) : "not set")
             << " - chargingProfilePurpose = "
             << (request.chargingProfilePurpose.isSet() ? ChargingProfilePurposeTypeHelper.toString(request.chargingProfilePurpose)
                                                        : "not set")
             << " - stackLevel = " << (request.stackLevel.isSet() ? std::to_string(request.stackLevel) : "not set");

    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    // Clear profiles
    if (m_profile_db.clear(request.id, request.connectorId, request.chargingProfilePurpose, request.stackLevel))
    {
        response.status = ClearChargingProfileStatus::Accepted;
    }
    else
    {
        response.status = ClearChargingProfileStatus::Unknown;
    }

    LOG_INFO << "Clear charging profile status : " << ClearChargingProfileStatusHelper.toString(response.status);

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::ocpp16::SetChargingProfileReq& request,
                                         ocpp::messages::ocpp16::SetChargingProfileConf&      response,
                                         std::string&                                         error_code,
                                         std::string&                                         error_message)
{
    bool ret = false;

    (void)error_code;

    LOG_INFO << "Set charging profile requested : chargingProfileId = " << request.csChargingProfiles.chargingProfileId
             << " - connectorId = " << request.connectorId << " - chargingProfilePurpose = "
             << ChargingProfilePurposeTypeHelper.toString(request.csChargingProfiles.chargingProfilePurpose)
             << " - stackLevel = " << request.csChargingProfiles.stackLevel;

    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    // Check connector
    Connector* connector = m_connectors.getConnector(request.connectorId);
    if (connector)
    {
        // Check level
        if (request.csChargingProfiles.stackLevel <= m_ocpp_config.chargeProfileMaxStackLevel())
        {
            // Check schedule
            if (request.csChargingProfiles.chargingSchedule.chargingSchedulePeriod.size() <= m_ocpp_config.chargingScheduleMaxPeriods())
            {
                // Check charging rate unit
                ChargingRateUnitType unit          = request.csChargingProfiles.chargingSchedule.chargingRateUnit;
                std::string          allowed_units = m_ocpp_config.chargingScheduleAllowedChargingRateUnit();
                if (((unit == ChargingRateUnitType::A) && (allowed_units.find("Current") != std::string::npos)) ||
                    ((unit == ChargingRateUnitType::W) && (allowed_units.find("Power") != std::string::npos)))
                {
                    // Check purpose
                    switch (request.csChargingProfiles.chargingProfilePurpose)
                    {
                        case ChargingProfilePurposeType::ChargePointMaxProfile:
                        {
                            // Chargepoint profile must always be on connector 0
                            if (request.connectorId == 0)
                            {
                                // Add profile
                                ret = true;
                            }
                            else
                            {
                                error_message = "Connector id must be 0";
                            }
                        }
                        break;

                        case ChargingProfilePurposeType::TxDefaultProfile:
                        {
                            // Add profile
                            ret = true;
                        }
                        break;

                        case ChargingProfilePurposeType::TxProfile:
                        // Intended fallthrough
                        default:
                        {
                            // Check if a transaction is in progress for the specific connector
                            if (connector->transaction_id != 0)
                            {
                                if (request.csChargingProfiles.transactionId.isSet())
                                {
                                    if (request.csChargingProfiles.transactionId.value() == connector->transaction_id)
                                    {
                                        // Add profile
                                        ret = true;
                                    }
                                }
                                else
                                {
                                    // Add profile
                                    ret = true;
                                }
                            }
                        }
                        break;
                    }
                    if (ret)
                    {
                        // Check kind
                        if ((request.csChargingProfiles.chargingProfileKind != ChargingProfileKindType::Recurring) ||
                            (request.csChargingProfiles.chargingSchedule.startSchedule.isSet() &&
                             request.csChargingProfiles.chargingSchedule.duration.isSet()))
                        {
                            // Install profile
                            ret = m_profile_db.install(request.connectorId, request.csChargingProfiles);
                            if (!ret)
                            {
                                error_message = "Number of charging profiles exceeds MaxChargingProfilesInstalled";
                            }
                        }
                        else
                        {
                            ret           = false;
                            error_message = "Recurring profiles must have a start schedule and a duration";
                        }
                    }
                }
                else
                {
                    error_message = "Unsupported charging rate unit";
                }
            }
            else
            {
                error_message = "Number of schedule periods exceeds ChargingScheduleMaxPeriods";
            }
        }
        else
        {
            error_message = "Stack level exceeds ChargeProfileMaxStackLevel";
        }
    }
    else
    {
        error_message = "Invalid connector id";
    }

    // Prepare response
    if (ret)
    {
        response.status = ChargingProfileStatus::Accepted;
        LOG_INFO << "Set charging profile status : Accepted";
    }
    else
    {
        response.status = ChargingProfileStatus::Rejected;
        LOG_INFO << "Set charging profile status rejected : " << error_message;
    }

    return true;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::ocpp16::GetCompositeScheduleReq& request,
                                         ocpp::messages::ocpp16::GetCompositeScheduleConf&      response,
                                         std::string&                                           error_code,
                                         std::string&                                           error_message)
{
    (void)error_code;
    (void)error_message;

    ocpp::types::ocpp16::ChargingRateUnitType charging_rate_unit;
    if (!request.chargingRateUnit.isSet())
    {
        if (m_ocpp_config.chargingScheduleAllowedChargingRateUnit().find("Power") != std::string::npos)
        {
            charging_rate_unit = ocpp::types::ocpp16::ChargingRateUnitType::W;
        }
        else
        {
            charging_rate_unit = ocpp::types::ocpp16::ChargingRateUnitType::A;
        }
    }
    else
    {
        charging_rate_unit = request.chargingRateUnit.value();
    }

    LOG_INFO << "GetCompositeSchedule requested : connectorId = " << request.connectorId << " - duration = " << request.duration
             << " - chargingRateUnit = " << ChargingRateUnitTypeHelper.toString(charging_rate_unit)
             << (!request.chargingRateUnit.isSet() ? " (from allowed charging unit)" : "");

    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    // Prepare response
    response.status = GetCompositeScheduleStatus::Rejected;

    // Get connector
    Connector* connector = m_connectors.getConnector(request.connectorId);
    if (connector)
    {
        // Get profiles list
        std::vector<const ProfileDatabase::ChargingProfileList*> profile_lists;
        if (connector->transaction_id != 0)
        {
            profile_lists.push_back(&m_profile_db.txProfiles());
        }
        profile_lists.push_back(&m_profile_db.txDefaultProfiles());

        // Compute periods
        std::vector<Period> periods;
        bool                error = false;
        DateTime            now   = DateTime::now();
        for (auto& profile_list : profile_lists)
        {
            unsigned int stack_level = std::numeric_limits<unsigned int>::max();
            for (const auto& profile : (*profile_list))
            {
                // Add the profile if it matches the selected connector
                // or take the profile installed for connector 0 if it doesn't exists
                if ((profile.first == request.connectorId) || ((profile.first == 0u) && (stack_level != profile.second.stackLevel)))
                {
                    std::vector<Period> profile_periods = getProfilePeriods(connector, profile.second, now, request.duration);
                    periods                             = mergeProfilePeriods(periods, profile_periods);
                    stack_level                         = profile.second.stackLevel;
                    if (periods.empty())
                    {
                        error = true;
                    }
                }
            }
            if (error)
            {
                break;
            }
        }
        if (periods.empty())
        {
            LOG_INFO << "No charging profiles for the requested period";
        }

        // Get local limitations
        ChargingProfile local_profile;
        local_profile.chargingProfileId      = 0;
        local_profile.chargingProfileKind    = ChargingProfileKindType::Absolute;
        local_profile.chargingProfilePurpose = ChargingProfilePurposeType::TxDefaultProfile;
        local_profile.stackLevel             = 0;
        if (m_events_handler.getLocalLimitationsSchedule(request.connectorId, request.duration, local_profile.chargingSchedule) &&
            !local_profile.chargingSchedule.chargingSchedulePeriod.empty())
        {
            // Ensure profile is absolute with the requested duration
            local_profile.chargingSchedule.startSchedule.clear();
            local_profile.chargingSchedule.startSchedule = now;
            local_profile.chargingSchedule.duration      = static_cast<int>(request.duration);

            // Merge periods
            std::vector<Period> local_periods = getProfilePeriods(connector, local_profile, now, request.duration);
            periods                           = mergeLocalPeriods(periods, local_periods);
        }
        else
        {
            LOG_INFO << "No local limitations for the requested period";
        }

        // Create response
        if (!periods.empty())
        {
            response.status        = GetCompositeScheduleStatus::Accepted;
            response.connectorId   = request.connectorId;
            response.scheduleStart = now;

            ChargingSchedule& schedule = response.chargingSchedule.value();
            schedule.duration          = 0;
            schedule.chargingRateUnit  = charging_rate_unit;

            // Adjust start if needed since first period must start at 0
            time_t offset          = periods[0].start;
            schedule.startSchedule = DateTime(now.timestamp() + offset);
            for (const auto& period : periods)
            {
                ChargingSchedulePeriod p;
                p.startPeriod = static_cast<int>(period.start - offset);
                if (period.unit == schedule.chargingRateUnit)
                {
                    p.limit = period.setpoint;
                }
                else
                {
                    p.limit = convertToUnit(period.setpoint, schedule.chargingRateUnit, period.nb_phases);
                }
                p.numberPhases = period.nb_phases;
                schedule.chargingSchedulePeriod.push_back(p);
                schedule.duration += static_cast<int>(period.duration);
            }
        }
        else
        {
            // No profiles, couldn't compute any schedule
        }
    }
    else
    {
        LOG_ERROR << "Invalid connector id : " << request.connectorId;
    }

    LOG_INFO << "GetCompositeSchedule status : " << GetCompositeScheduleStatusHelper.toString(response.status);

    return true;
}

/** @brief Periodically cleanup expired profiles */
void SmartChargingManager::cleanupProfiles()
{
    // Lock profiles
    std::lock_guard<std::mutex> lock(m_mutex);

    // List of profiles to erase
    std::vector<int> profiles_to_delete;

    // Check validity date of all installed profiles
    DateTime now = DateTime::now();
    for (const auto profiles_list : {&m_profile_db.chargePointMaxProfiles(), &m_profile_db.txDefaultProfiles(), &m_profile_db.txProfiles()})
    {
        for (const auto& profile : (*profiles_list))
        {
            if (profile.second.validTo.isSet() && (profile.second.validTo.value() < now))
            {
                profiles_to_delete.push_back(profile.second.chargingProfileId);
            }
            else
            {
                // For absolute profiles, check schedule validity
                if ((profile.second.chargingProfileKind == ChargingProfileKindType::Absolute) &&
                    profile.second.chargingSchedule.startSchedule.isSet() && profile.second.chargingSchedule.duration.isSet())
                {
                    if ((profile.second.chargingSchedule.startSchedule.value() + profile.second.chargingSchedule.duration.value()) < now)
                    {
                        profiles_to_delete.push_back(profile.second.chargingProfileId);
                    }
                }
            }
        }
    }

    // Check Tx profiles not associated with ongoing transaction
    for (const auto& profile : m_profile_db.txProfiles())
    {
        if (profile.second.transactionId.isSet())
        {
            Connector* connector = m_connectors.getConnector(profile.first);
            if (connector && (connector->transaction_id != profile.second.transactionId))
            {
                profiles_to_delete.push_back(profile.second.chargingProfileId);
            }
        }
    }

    // Erase profiles
    for (int profile : profiles_to_delete)
    {
        m_profile_db.clear(profile);
    }
}

/** @brief Compute the setpoint of a given connector with a profile list */
void SmartChargingManager::computeSetpoint(Connector*                                                         connector,
                                           ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& connector_setpoint,
                                           ocpp::types::ocpp16::ChargingRateUnitType                          unit,
                                           const ProfileDatabase::ChargingProfileList&                        profiles_list)
{
    for (const auto& profile : profiles_list)
    {
        // Check connector
        if ((profile.first == connector->id) || (profile.first == 0))
        {
            // Check if the profile is active
            size_t period = 0;
            if (isProfileActive(connector, profile.second, period, DateTime::now()))
            {
                // Apply setpoint
                fillSetpoint(connector_setpoint, unit, profile.second, profile.second.chargingSchedule.chargingSchedulePeriod[period]);

                // Highest applicable profile found
                break;
            }
        }
    }
}

/** @brief Check if the given profile is active */
bool SmartChargingManager::isProfileActive(Connector*                                  connector,
                                           const ocpp::types::ocpp16::ChargingProfile& profile,
                                           size_t&                                     period,
                                           const ocpp::types::DateTime&                time_point)
{
    bool ret = false;

    // Check profile validity
    if (isProfileValid(profile, time_point))
    {
        // Get profile kind
        ChargingProfileKindType kind = getProfileKind(profile);

        // Compute start of schedule
        DateTime start_of_schedule = getProfileStartTime(connector, profile, kind, time_point);

        // Check schedule validity
        if ((start_of_schedule <= time_point) &&
            (!profile.chargingSchedule.duration.isSet() || ((start_of_schedule + profile.chargingSchedule.duration) >= time_point)))
        {
            // Look for the matching period
            const auto& schedule_periods = profile.chargingSchedule.chargingSchedulePeriod;
            period                       = schedule_periods.size() - 1u;
            for (auto iter = schedule_periods.rbegin(); iter != schedule_periods.rend(); iter++)
            {
                if ((start_of_schedule + iter->startPeriod) <= time_point)
                {
                    ret = true;
                    break;
                }
                else
                {
                    period--;
                }
            }
        }
    }

    return ret;
}

/** @brief Fill a setpoint structure with a charging profile and a charging schedule period */
void SmartChargingManager::fillSetpoint(ocpp::types::ocpp16::SmartChargingSetpoint&        setpoint,
                                        ocpp::types::ocpp16::ChargingRateUnitType          unit,
                                        const ocpp::types::ocpp16::ChargingProfile&        profile,
                                        const ocpp::types::ocpp16::ChargingSchedulePeriod& period)
{
    setpoint.min_charging_rate = profile.chargingSchedule.minChargingRate;
    if (period.numberPhases.isSet())
    {
        setpoint.number_phases = period.numberPhases;
    }
    else
    {
        setpoint.number_phases = 3u; // Default, if not set is 3 phases charging
    }
    if (profile.chargingSchedule.chargingRateUnit == unit)
    {
        setpoint.value = period.limit;
    }
    else
    {
        setpoint.value = convertToUnit(period.limit, unit, setpoint.number_phases);
        if (setpoint.min_charging_rate.isSet())
        {
            setpoint.min_charging_rate = convertToUnit(setpoint.min_charging_rate, unit, setpoint.number_phases);
        }
    }
}

/** @brief Convert charging rate units */
float SmartChargingManager::convertToUnit(float value, ocpp::types::ocpp16::ChargingRateUnitType unit, unsigned int number_phases)
{
    float ret;
    if (unit == ChargingRateUnitType::A)
    {
        ret = value / (static_cast<float>(number_phases) * m_stack_config.operatingVoltage());
    }
    else
    {
        ret = value * static_cast<float>(number_phases) * m_stack_config.operatingVoltage();
    }
    return ret;
}

/** @brief Indicate if a charging profile is valid at a given point in time */
bool SmartChargingManager::isProfileValid(const ocpp::types::ocpp16::ChargingProfile& profile, const ocpp::types::DateTime& time_point)
{
    bool ret = false;
    if ((!profile.validFrom.isSet() || (time_point >= profile.validFrom)) && (!profile.validTo.isSet() || (time_point <= profile.validTo)))
    {
        ret = true;
    }
    return ret;
}

/** @brief Get the kind of charging profile */
ocpp::types::ocpp16::ChargingProfileKindType SmartChargingManager::getProfileKind(const ocpp::types::ocpp16::ChargingProfile& profile)
{
    ChargingProfileKindType kind = profile.chargingProfileKind;
    if (kind == ChargingProfileKindType::Absolute)
    {
        // Specific case of Absolute schedule : if startSchedule field is not set,
        // the schedule is actually a Relative schedule
        if (!profile.chargingSchedule.startSchedule.isSet())
        {
            kind = ChargingProfileKindType::Relative;
        }
    }
    return kind;
}

/** @brief Get the start time of a charging profile schedule */
ocpp::types::DateTime SmartChargingManager::getProfileStartTime(Connector*                                   connector,
                                                                const ocpp::types::ocpp16::ChargingProfile&  profile,
                                                                ocpp::types::ocpp16::ChargingProfileKindType kind,
                                                                const ocpp::types::DateTime&                 time_point)
{
    DateTime start_of_schedule;
    switch (kind)
    {
        case ChargingProfileKindType::Recurring:
        {
            // Get start of schedule day of the week and time of the day
            std::tm tm_start_schedule;
            time_t  start_schedule_time_t = profile.chargingSchedule.startSchedule.value().timestamp();
#ifdef _MSC_VER
            localtime_s(&tm_start_schedule, &start_schedule_time_t);
#else  // _MSC_VER
            localtime_r(&start_schedule_time_t, &tm_start_schedule);
#endif // _MSC_VER

            // Get the same information on today
            std::tm tm_today;
            time_t  now_time_t = time_point.timestamp();
#ifdef _MSC_VER
            localtime_s(&tm_today, &now_time_t);
#else  // _MSC_VER
            localtime_r(&now_time_t, &tm_today);
#endif // _MSC_VER

            // Apply the same time on today
            tm_today.tm_hour = tm_start_schedule.tm_hour;
            tm_today.tm_min  = tm_start_schedule.tm_min;
            tm_today.tm_sec  = tm_start_schedule.tm_sec;

            // Compute start of schedule for today
            start_of_schedule = mktime(&tm_today);

            // Compute recurrency to obtain the start of the schedule
            if (profile.recurrencyKind == RecurrencyKindType::Weekly)
            {
                // Weekly

                // Compare day of the week
                if (tm_start_schedule.tm_wday != tm_today.tm_wday)
                {
                    // Not the good day, compute the start of schedule
                    // based on the number of days before the actual scheduled day
                    int days_count = tm_start_schedule.tm_wday - tm_today.tm_wday;
                    if (days_count < 0)
                    {
                        days_count = tm_today.tm_wday - tm_start_schedule.tm_wday;
                    }
                    start_of_schedule =
                        DateTime(start_of_schedule.timestamp() + days_count * 86400); // 86400 = 3600 * 24 = Number of seconds in a day
                }
            }
        }
        break;

        case ChargingProfileKindType::Absolute:
        {
            // Start of schedule is defined in the profile itself
            start_of_schedule = profile.chargingSchedule.startSchedule;
        }
        break;

        case ChargingProfileKindType::Relative:
        {
            // Check if a transaction is in progress
            if (connector->transaction_id != 0)
            {
                // Start of schedule is the start of the transaction
                start_of_schedule = connector->transaction_start;
            }
            else
            {
                // Start of schedule is the given time point
                start_of_schedule = time_point;
            }
        }
        break;
    }
    return start_of_schedule;
}

/** @brief Get the composite schedule periods corresponding to a charging profile */
std::vector<SmartChargingManager::Period> SmartChargingManager::getProfilePeriods(Connector*                                  connector,
                                                                                  const ocpp::types::ocpp16::ChargingProfile& profile,
                                                                                  const ocpp::types::DateTime&                time_point,
                                                                                  unsigned int                                duration)
{
    std::vector<Period> periods;

    // Current time point
    time_t ts_time_point = time_point.timestamp();

    // Compute start of schedule
    ChargingProfileKindType kind                 = getProfileKind(profile);
    DateTime                start_of_schedule    = getProfileStartTime(connector, profile, kind, time_point);
    time_t                  ts_start_of_schedule = start_of_schedule.timestamp();

    // Compute the end of schedule
    DateTime end_of_schedule    = DateTime(time_point.timestamp() + duration);
    time_t   ts_end_of_schedule = end_of_schedule.timestamp();
    if (profile.chargingSchedule.duration.isSet())
    {
        time_t ts_profile_end_of_schedule = ts_start_of_schedule + profile.chargingSchedule.duration;
        if (ts_profile_end_of_schedule < ts_end_of_schedule)
        {
            ts_end_of_schedule = ts_profile_end_of_schedule;
        }
    }

    // Check if the profile is active
    bool   found  = false;
    size_t period = 0;
    if (!profile.chargingSchedule.chargingSchedulePeriod.empty())
    {
        if (isProfileActive(connector, profile, period, time_point))
        {
            // Profile is active
            found = true;
        }
        else
        {
            // Check start of schedule
            if ((start_of_schedule <= ts_end_of_schedule) && isProfileValid(profile, start_of_schedule))
            {
                // Get the first period
                period = 0;
                found  = true;
            }
        }
    }

    // Check if a period has been found
    if (found)
    {
        // Look for schedule periods
        bool   end           = false;
        time_t delta_start   = ts_start_of_schedule - ts_time_point;
        time_t current_start = ((delta_start > 0) ? delta_start : 0);
        do
        {
            // Current schedule period
            const ChargingSchedulePeriod& schedule_period = profile.chargingSchedule.chargingSchedulePeriod[period];

            // Create composite schedule period
            Period p;
            p.start    = current_start;
            p.setpoint = schedule_period.limit;
            p.unit     = profile.chargingSchedule.chargingRateUnit;
            if (schedule_period.numberPhases.isSet())
            {
                p.nb_phases = schedule_period.numberPhases;
            }
            else
            {
                p.nb_phases = 3u; // Default, if not set is 3 phases charging
            }

            // Next period
            period++;

            // Compute duration
            if (period == profile.chargingSchedule.chargingSchedulePeriod.size())
            {
                // Last period
                p.duration = ts_end_of_schedule - (ts_time_point + p.start);
                end        = true;
            }
            else
            {
                p.duration = profile.chargingSchedule.chargingSchedulePeriod[period].startPeriod + delta_start - p.start;
                if ((p.start + p.duration + ts_time_point) >= ts_end_of_schedule)
                {
                    // The current period ends after the requested schedule end
                    p.duration = ts_end_of_schedule - (p.start + ts_time_point);
                    end        = true;
                }
                current_start = p.start + p.duration;
            }

            // Add period
            periods.push_back(p);
        } while (!end);
    }

    return periods;
}

/** @brief Merge charging profiles periods */
std::vector<SmartChargingManager::Period> SmartChargingManager::mergeProfilePeriods(const std::vector<Period>& ref_periods,
                                                                                    const std::vector<Period>& new_periods)
{
    std::vector<Period> merged_periods;

    // Check if a merge is needed
    if (ref_periods.empty())
    {
        merged_periods = new_periods;
    }
    else
    {
        bool   error            = false;
        size_t ref_period_index = 0;
        for (size_t i = 0; (i < new_periods.size()) && !error; i++)
        {
            // Check if there are reference periods left
            if (ref_period_index != ref_periods.size())
            {
                // Check if the new period is beginning before the reference period
                if (new_periods[i].start < ref_periods[ref_period_index].start)
                {
                    if ((new_periods[i].start + new_periods[i].duration) < ref_periods[ref_period_index].start)
                    {
                        // The whole period is before the reference period
                        merged_periods.push_back(new_periods[i]);
                    }
                    else
                    {
                        // A part of the period which overlapse the reference period
                        Period p;
                        p.start     = new_periods[i].start;
                        p.duration  = ref_periods[ref_period_index].start - new_periods[i].start;
                        p.setpoint  = new_periods[i].setpoint;
                        p.unit      = new_periods[i].unit;
                        p.nb_phases = new_periods[i].nb_phases;
                        merged_periods.push_back(p);

                        // Add all following consecutive reference periods
                        const Period* previous = &p;
                        while (ref_period_index != ref_periods.size())
                        {
                            const Period& ref_period = ref_periods[ref_period_index];
                            if (ref_period.start == (previous->start + previous->duration))
                            {
                                merged_periods.push_back(ref_period);
                                previous = &merged_periods.back();
                                ref_period_index++;
                            }
                            else
                            {
                                // Non consecutive period
                                error = true;
                                break;
                            }
                        }

                        // Check if the new period is still enabled
                        if ((new_periods[i].start + new_periods[i].duration) > (previous->start + previous->duration))
                        {
                            // Restart computation for this period
                            i--;
                        }
                    }
                }
                else
                {
                    // Add all following consecutive reference periods
                    const Period* previous = nullptr;
                    while (ref_period_index != ref_periods.size())
                    {
                        const Period& ref_period = ref_periods[ref_period_index];
                        if (!previous || (ref_period.start == (previous->start + previous->duration)))
                        {
                            merged_periods.push_back(ref_period);
                            previous = &merged_periods.back();
                            ref_period_index++;
                        }
                        else
                        {
                            // Non consecutive period
                            error = true;
                            break;
                        }
                    }

                    // Check if the new period is still enabled
                    if (previous)
                    {
                        if ((new_periods[i].start + new_periods[i].duration) > (previous->start + previous->duration))
                        {
                            // Restart computation for this period
                            i--;
                        }
                    }
                }
            }
            else
            {
                // No more reference periods, add the new periods at the end
                const Period* last_period = &merged_periods.back();
                if (((new_periods[i].start + new_periods[i].duration) > (last_period->start + last_period->duration)) &&
                    ((new_periods[i].start < last_period->start) || (new_periods[i].start < (last_period->start + last_period->duration))))
                {
                    // Add the susbset of period after the last period
                    Period p;
                    p.start     = last_period->start + last_period->duration;
                    p.duration  = new_periods[i].duration - (p.start - new_periods[i].start);
                    p.setpoint  = new_periods[i].setpoint;
                    p.unit      = new_periods[i].unit;
                    p.nb_phases = new_periods[i].nb_phases;
                    merged_periods.push_back(p);
                }
                else
                {
                    // Add the whole period
                    if (new_periods[i].start == (last_period->start + last_period->duration))
                    {
                        merged_periods.push_back(new_periods[i]);
                    }
                    else
                    {
                        if (new_periods[i].start > (last_period->start + last_period->duration))
                        {
                            // Non consecutive period
                            error = true;
                        }
                    }
                }
            }
        }
        if (error || (ref_period_index != ref_periods.size()))
        {
            LOG_WARNING << "Unable to compute the composite schedule due to non continuous profiles periods";
            merged_periods.clear();
        }
    }

    return merged_periods;
}

/** @brief Merge local limitations periods */
std::vector<SmartChargingManager::Period> SmartChargingManager::mergeLocalPeriods(const std::vector<Period>& profiles_periods,
                                                                                  const std::vector<Period>& local_periods)
{
    std::vector<Period> merged_periods;

    // Check if a merge is needed
    if (profiles_periods.empty())
    {
        merged_periods = local_periods;
    }
    else
    {
        bool   offset                    = false;
        time_t local_period_start        = 0;
        time_t local_period_start_offset = 0;
        size_t profiles_period_index     = 0;
        for (size_t i = 0; i < local_periods.size(); i++)
        {
            // Compute start of current local period
            if (offset)
            {
                offset = false;
            }
            else
            {
                local_period_start        = local_periods[i].start;
                local_period_start_offset = 0;
            }

            time_t local_period_end = local_period_start + (local_periods[i].duration - local_period_start_offset);
            // Check if there are profiles periods left
            if (profiles_period_index != profiles_periods.size())
            {
                if (local_period_end <= profiles_periods[profiles_period_index].start)
                {
                    // The whole period is before the profile period
                    addMergedPeriod(local_periods[i], merged_periods);
                }
                else if (local_period_start >= profiles_periods[profiles_period_index].start)
                {
                    time_t profiles_period_end =
                        profiles_periods[profiles_period_index].start + profiles_periods[profiles_period_index].duration;
                    if (local_period_end <= profiles_period_end)
                    {
                        // The whole period is included inside the profile period
                        Period p;
                        p.start    = local_period_start;
                        p.duration = local_periods[i].duration - local_period_start_offset;
                        mergeSetpoint(profiles_periods[profiles_period_index], local_periods[i], p);
                        addMergedPeriod(p, merged_periods);
                    }
                    else
                    {
                        // The period is across profile periods
                        Period p;
                        p.start    = local_period_start;
                        p.duration = profiles_period_end - local_period_start;
                        mergeSetpoint(profiles_periods[profiles_period_index], local_periods[i], p);
                        addMergedPeriod(p, merged_periods);

                        // Next profiles period but keep working on the same local period
                        local_period_start        = p.start + p.duration;
                        local_period_start_offset = local_period_start - local_periods[i].start;
                        offset                    = true;
                        profiles_period_index++;
                        i--;
                    }
                }
                else
                {
                    // A part of the local period which overlapse the profile period
                    Period p;
                    p.start     = local_periods[i].start;
                    p.duration  = profiles_periods[profiles_period_index].start - local_periods[i].start;
                    p.setpoint  = local_periods[i].setpoint;
                    p.unit      = local_periods[i].unit;
                    p.nb_phases = local_periods[i].nb_phases;
                    addMergedPeriod(p, merged_periods);

                    // Keep working on the same local period
                    local_period_start        = p.start + p.duration;
                    local_period_start_offset = local_period_start - local_periods[i].start;
                    offset                    = true;
                    i--;
                }
            }
            else
            {
                // A local period without merge
                Period p;
                p.start     = local_period_start;
                p.duration  = local_periods[i].duration - local_period_start_offset;
                p.setpoint  = local_periods[i].setpoint;
                p.unit      = local_periods[i].unit;
                p.nb_phases = local_periods[i].nb_phases;
                addMergedPeriod(p, merged_periods);
            }
        }
    }
    return merged_periods;
}

/** @brief Merge the setpoint of a local limitation and a profile limitation */
void SmartChargingManager::mergeSetpoint(const Period& profile_period, const Period& local_period, Period& merged_period)
{
    float profile_setpoint = profile_period.setpoint;
    if (profile_period.unit != local_period.unit)
    {
        profile_setpoint = convertToUnit(profile_period.setpoint, local_period.unit, profile_period.nb_phases);
    }
    if (local_period.setpoint < profile_setpoint)
    {
        merged_period.setpoint  = local_period.setpoint;
        merged_period.unit      = local_period.unit;
        merged_period.nb_phases = local_period.nb_phases;
    }
    else
    {
        merged_period.setpoint  = profile_period.setpoint;
        merged_period.unit      = profile_period.unit;
        merged_period.nb_phases = profile_period.nb_phases;
    }
}

/** @brief Add a local limitation and profiles limitations merged period */
void SmartChargingManager::addMergedPeriod(const Period& merged_period, std::vector<Period>& periods)
{
    if (periods.empty())
    {
        periods.push_back(merged_period);
    }
    else
    {
        Period& last_period = periods.back();
        if ((last_period.setpoint == merged_period.setpoint) && (last_period.unit == merged_period.unit) &&
            (last_period.nb_phases == merged_period.nb_phases))
        {
            last_period.duration += merged_period.duration;
        }
        else
        {
            periods.push_back(merged_period);
        }
    }
}

} // namespace chargepoint
} // namespace ocpp
