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
                                           ocpp::helpers::TimerPool&                       timer_pool,
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
            const ChargingSchedulePeriod* period = nullptr;
            if (isProfileActive(connector, profile.second, period))
            {
                // Apply setpoint
                fillSetpoint(charge_point_setpoint, unit, profile.second, *period);
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
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::ClearChargingProfileReq& request,
                                         ocpp::messages::ClearChargingProfileConf&      response,
                                         const char*&                                   error_code,
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
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::SetChargingProfileReq& request,
                                         ocpp::messages::SetChargingProfileConf&      response,
                                         const char*&                                 error_code,
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
 *                                                                                const char*& error_code,
 *                                                                                std::string& error_message)
 */
bool SmartChargingManager::handleMessage(const ocpp::messages::GetCompositeScheduleReq& request,
                                         ocpp::messages::GetCompositeScheduleConf&      response,
                                         const char*&                                   error_code,
                                         std::string&                                   error_message)
{
    (void)request;
    (void)error_code;
    (void)error_message;

    LOG_INFO << "GetCompositeSchedule requested : connectorId = " << request.connectorId << " - duration = " << request.duration
             << " - chargingRateUnit = "
             << (request.chargingRateUnit.isSet() ? ChargingRateUnitTypeHelper.toString(request.chargingRateUnit) : "not set");

    // Not supported for now...
    response.status = GetCompositeScheduleStatus::Rejected;

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
            const ChargingSchedulePeriod* period = nullptr;
            if (isProfileActive(connector, profile.second, period))
            {
                // Apply setpoint
                fillSetpoint(connector_setpoint, unit, profile.second, *period);
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
bool SmartChargingManager::isProfileActive(Connector*                                  connector,
                                           const ocpp::types::ChargingProfile&         profile,
                                           const ocpp::types::ChargingSchedulePeriod*& period)
{
    bool ret = false;

    // Check profile validity
    DateTime now = DateTime::now();
    if ((!profile.validFrom.isSet() || (now >= profile.validFrom)) && (!profile.validTo.isSet() || (now <= profile.validTo)))
    {
        // Check profile kind
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

        // Compute start of schedule
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
                time_t  now_time_t = now.timestamp();
                localtime_r(&now_time_t, &tm_today);

                // Compute recurrency to obtain the start of the schedule
                if (profile.recurrencyKind == RecurrencyKindType::Daily)
                {
                    // Daily

                    // Apply the same time on today
                    tm_today.tm_hour = tm_start_schedule.tm_hour;
                    tm_today.tm_min  = tm_start_schedule.tm_min;
                    tm_today.tm_sec  = tm_start_schedule.tm_sec;

                    // Compute start of schedule for today
                    start_of_schedule = mktime(&tm_today);
                }
                else
                {
                    // Weekly

                    // Compare day of the week
                    if (tm_start_schedule.tm_wday == tm_today.tm_wday)
                    {
                        // Apply the same time on today
                        tm_today.tm_hour = tm_start_schedule.tm_hour;
                        tm_today.tm_min  = tm_start_schedule.tm_min;
                        tm_today.tm_sec  = tm_start_schedule.tm_sec;

                        // Compute start of schedule for today
                        start_of_schedule = mktime(&tm_today);
                    }
                    else
                    {
                        // Not the good day, put the start of schedule in the future
                        // to have it discard
                        start_of_schedule = now + 1;
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
                // Start of schedule is the start of the transaction
                start_of_schedule = connector->transaction_start;
            }
            break;
        }

        // Check schedule validity
        if ((start_of_schedule <= now) &&
            (!profile.chargingSchedule.duration.isSet() || ((start_of_schedule + profile.chargingSchedule.duration) >= now)))
        {
            // Look for the matching period
            const auto& schedule_periods = profile.chargingSchedule.chargingSchedulePeriod;
            for (auto iter = schedule_periods.rbegin(); iter != schedule_periods.rend(); iter++)
            {
                if ((start_of_schedule + iter->startPeriod) <= now)
                {
                    period = &(*iter);
                    ret    = true;
                    break;
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

} // namespace chargepoint
} // namespace ocpp
