/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
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
#include "IOcppConfig.h"
#include "Logger.h"
#include "WorkerThreadPool.h"

#include <algorithm>

using namespace ocpp::types;
using namespace ocpp::messages;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
SmartChargingManager::SmartChargingManager(const ocpp::config::IChargePointConfig&         stack_config,
                                           ocpp::config::IOcppConfig&                      ocpp_config,
                                           ocpp::database::Database&                       database,
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
                                                     ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>&,
                                                     ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>&,
                                                     ocpp::types::ChargingRateUnitType) */
bool SmartChargingManager::getSetpoint(unsigned int                                               connector_id,
                                       ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& charge_point_setpoint,
                                       ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& connector_setpoint,
                                       ocpp::types::ChargingRateUnitType                          unit)
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

        // Compute connector setpoint if a transaction is active on the connector
        connector_setpoint.clear();
        if (connector->transaction_id != 0)
        {
            computeSetpoint(connector, connector_setpoint, unit, m_profile_db.txProfiles());
            if (!connector_setpoint.isSet())
            {
                computeSetpoint(connector, connector_setpoint, unit, m_profile_db.txDefaultProfiles());
            }
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

/** @copydoc bool ISmartChargingManager::installTxProfile(unsigned int, const ocpp::types::ChargingProfile&) */
bool SmartChargingManager::installTxProfile(unsigned int connector_id, const ocpp::types::ChargingProfile& profile)
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
bool SmartChargingManager::handleMessage(const ocpp::messages::ClearChargingProfileReq& request,
                                         ocpp::messages::ClearChargingProfileConf&      response,
                                         std::string&                                   error_code,
                                         std::string&                                   error_message)
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
bool SmartChargingManager::handleMessage(const ocpp::messages::SetChargingProfileReq& request,
                                         ocpp::messages::SetChargingProfileConf&      response,
                                         std::string&                                 error_code,
                                         std::string&                                 error_message)
{
    bool ret = false;

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
            if (request.csChargingProfiles.chargingSchedule.chargingSchedulePeriod.size() <= m_ocpp_config.chargeProfileMaxStackLevel())
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
                                // Add profile
                                ret = true;
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
    }
    else
    {
        error_code      = ocpp::rpc::IRpc::RPC_ERROR_PROPERTY_CONSTRAINT_VIOLATION;
        response.status = ChargingProfileStatus::Rejected;
    }

    LOG_INFO << "Set charging profile status : " << ChargingProfileStatusHelper.toString(response.status);

    return ret;
}

/** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
 *                                                                                ResponseType& response,
 *                                                                                std::string& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::GetCompositeScheduleReq& request,
                                         ocpp::messages::GetCompositeScheduleConf&      response,
                                         std::string&                                   error_code,
                                         std::string&                                   error_message)
{
    bool ret = false;
    (void)error_code;

    LOG_INFO << "GetCompositeSchedule requested : connectorId = " << request.connectorId << " - duration = " << request.duration
             << " - chargingRateUnit = "
             << (request.chargingRateUnit.isSet() ? ChargingRateUnitTypeHelper.toString(request.chargingRateUnit) : "not set");

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
        DateTime            now = DateTime::now();
        for (auto& profile_list : profile_lists)
        {
            for (const auto& profile : (*profile_list))
            {
                if (profile.first == request.connectorId)
                {
                    std::vector<Period> profile_periods = getProfilePeriods(connector, profile.second, now, request.duration);
                    periods                             = mergeProfilePeriods(periods, profile_periods);
                    if (periods.empty())
                    {
                        break;
                    }
                }
            }
            if (periods.empty())
            {
                break;
            }
        }

        // Create response
        if (!periods.empty())
        {
            response.status        = GetCompositeScheduleStatus::Accepted;
            response.connectorId   = request.connectorId;
            response.scheduleStart = now;

            ChargingSchedule& schedule = response.chargingSchedule.value();
            schedule.duration          = 0;
            if (request.chargingRateUnit.isSet())
            {
                schedule.chargingRateUnit = request.chargingRateUnit;
            }
            else
            {
                schedule.chargingRateUnit = ChargingRateUnitType::A;
            }
            // Adjust start if needed since first period must start at 0
            int offset             = periods[0].start;
            schedule.startSchedule = DateTime(now.timestamp() + offset);
            for (const auto& period : periods)
            {
                ChargingSchedulePeriod p;
                p.startPeriod = period.start - offset;
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
                schedule.duration += period.duration;
            }
        }
        else
        {
            // No profiles, couldn't compute any schedule
            response.status = GetCompositeScheduleStatus::Rejected;
        }

        ret = true;
    }
    else
    {
        error_message = "Invalid connector id";
    }

    LOG_INFO << "GetCompositeSchedule status : " << GetCompositeScheduleStatusHelper.toString(response.status);

    return ret;
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
void SmartChargingManager::computeSetpoint(Connector*                                                 connector,
                                           ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& connector_setpoint,
                                           ocpp::types::ChargingRateUnitType                          unit,
                                           const ProfileDatabase::ChargingProfileList&                profiles_list)
{
    unsigned int level = 0;
    for (const auto& profile : profiles_list)
    {
        // Check if the profile has been found
        if (connector_setpoint.isSet() && (profile.second.stackLevel < level))
        {
            // Profile found
            break;
        }

        // Check connector
        if ((profile.first == connector->id) || (profile.first == 0))
        {
            // Check if the profile is active
            size_t period = 0;
            if (isProfileActive(connector, profile.second, period, DateTime::now()))
            {
                // Apply setpoint
                fillSetpoint(connector_setpoint, unit, profile.second, profile.second.chargingSchedule.chargingSchedulePeriod[period]);
            }

            // Check connector type
            if (profile.first == 0)
            {
                // Any connector profile, save stack level in case of a connector specific
                // profile with the same stack level exists
                level = profile.second.stackLevel;
            }
            else
            {
                // Connector specific profile, stop search since it has highest priority
                // over any connector profile
                break;
            }
        }
    }
}

/** @brief Check if the given profile is active */
bool SmartChargingManager::isProfileActive(Connector*                          connector,
                                           const ocpp::types::ChargingProfile& profile,
                                           size_t&                             period,
                                           const ocpp::types::DateTime&        time_point)
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
            (!profile.chargingSchedule.duration.isSet() || ((start_of_schedule + profile.chargingSchedule.duration) <= time_point)))
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
void SmartChargingManager::fillSetpoint(ocpp::types::SmartChargingSetpoint&        setpoint,
                                        ocpp::types::ChargingRateUnitType          unit,
                                        const ocpp::types::ChargingProfile&        profile,
                                        const ocpp::types::ChargingSchedulePeriod& period)
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
float SmartChargingManager::convertToUnit(float value, ocpp::types::ChargingRateUnitType unit, unsigned int number_phases)
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
bool SmartChargingManager::isProfileValid(const ocpp::types::ChargingProfile& profile, const ocpp::types::DateTime& time_point)
{
    bool ret = false;
    if ((!profile.validFrom.isSet() || (time_point >= profile.validFrom)) && (!profile.validTo.isSet() || (time_point <= profile.validTo)))
    {
        ret = true;
    }
    return ret;
}

/** @brief Get the kind of charging profile */
ocpp::types::ChargingProfileKindType SmartChargingManager::getProfileKind(const ocpp::types::ChargingProfile& profile)
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
ocpp::types::DateTime SmartChargingManager::getProfileStartTime(Connector*                           connector,
                                                                const ocpp::types::ChargingProfile&  profile,
                                                                ocpp::types::ChargingProfileKindType kind,
                                                                const ocpp::types::DateTime&         time_point)
{
    DateTime start_of_schedule;
    switch (kind)
    {
        case ChargingProfileKindType::Recurring:
        {
            // Get start of schedule day of the week and time of the day
            std::tm tm_start_schedule;
            time_t  start_schedule_time_t = profile.chargingSchedule.startSchedule.value().timestamp();
            localtime_r(&start_schedule_time_t, &tm_start_schedule);

            // Get the same information on today
            std::tm tm_today;
            time_t  now_time_t = time_point.timestamp();
            localtime_r(&now_time_t, &tm_today);

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
std::vector<SmartChargingManager::Period> SmartChargingManager::getProfilePeriods(Connector*                          connector,
                                                                                  const ocpp::types::ChargingProfile& profile,
                                                                                  const ocpp::types::DateTime&        time_point,
                                                                                  unsigned int                        duration)
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
                p.duration    = profile.chargingSchedule.chargingSchedulePeriod[period].startPeriod + delta_start - p.start;
                current_start = p.start + p.duration;
            }

            // Add period
            periods.push_back(p);
        } while (!end);
    }

    return periods;
}

/** @brief Merge composite schedule periods */
std::vector<SmartChargingManager::Period> SmartChargingManager::mergeProfilePeriods(const std::vector<Period>& ref_periods,
                                                                                    const std::vector<Period>& new_periods)
{
    std::vector<Period> merged_periods;

    // Check first merge
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
                    if ((new_periods[i].start + new_periods[i].duration) > (previous->start + previous->duration))
                    {
                        // Restart computation for this period
                        i--;
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
            LOG_ERROR << "Unable to compute the composite schedule due to non continuous profiles periods";
            merged_periods.clear();
        }
    }

    return merged_periods;
}

} // namespace chargepoint
} // namespace ocpp
