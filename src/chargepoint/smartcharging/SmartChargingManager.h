/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_SMARTCHARGINGMANAGER_H
#define OPENOCPP_SMARTCHARGINGMANAGER_H

#include "ClearChargingProfile.h"
#include "Enums.h"
#include "GenericMessageHandler.h"
#include "GetCompositeSchedule.h"
#include "ISmartChargingManager.h"
#include "ProfileDatabase.h"
#include "SetChargingProfile.h"
#include "Timer.h"

#include <mutex>

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
class IChargePointConfig;
} // namespace config
namespace messages
{
class IMessageDispatcher;
class GenericMessagesConverter;
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class Connectors;
struct Connector;
class IChargePointEventsHandler;

/** @brief Handle smart charging for the charge point */
class SmartChargingManager
    : public ISmartChargingManager,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ClearChargingProfileReq, ocpp::messages::ClearChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::SetChargingProfileReq, ocpp::messages::SetChargingProfileConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::GetCompositeScheduleReq, ocpp::messages::GetCompositeScheduleConf>

{
  public:
    /** @brief Constructor */
    SmartChargingManager(const ocpp::config::IChargePointConfig&         stack_config,
                         ocpp::config::IOcppConfig&                      ocpp_config,
                         ocpp::database::Database&                       database,
                         IChargePointEventsHandler&                      events_handler,
                         ocpp::helpers::ITimerPool&                      timer_pool,
                         ocpp::helpers::WorkerThreadPool&                worker_pool,
                         Connectors&                                     connectors,
                         const ocpp::messages::GenericMessagesConverter& messages_converter,
                         ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~SmartChargingManager();

    // ISmartChargingManager interface

    /** @copydoc bool ISmartChargingManager::getSetpoint(unsigned int,
                                                         ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>&,
                                                         ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>&,
                                                         ocpp::types::ChargingRateUnitType) */
    bool getSetpoint(unsigned int                                               connector_id,
                     ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& charge_point_setpoint,
                     ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& connector_setpoint,
                     ocpp::types::ChargingRateUnitType                          unit) override;

    /** @copydoc bool ISmartChargingManager::installTxProfile(unsigned int, const ocpp::types::ChargingProfile&) */
    bool installTxProfile(unsigned int connector_id, const ocpp::types::ChargingProfile& profile) override;

    /** @copydoc void ISmartChargingManager::assignPendingTxProfiles(unsigned int), unsigned int) */
    void assignPendingTxProfiles(unsigned int connector_id, int transaction_id) override;

    /** @copydoc void ISmartChargingManager::clearTxProfiles(unsigned int) */
    void clearTxProfiles(unsigned int connector_id) override;

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ClearChargingProfileReq& request,
                       ocpp::messages::ClearChargingProfileConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::SetChargingProfileReq& request,
                       ocpp::messages::SetChargingProfileConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::GetCompositeScheduleReq& request,
                       ocpp::messages::GetCompositeScheduleConf&      response,
                       std::string&                                   error_code,
                       std::string&                                   error_message) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Connectors */
    Connectors& m_connectors;

    /** @brief Profile database */
    ProfileDatabase m_profile_db;

    /** @brief Protect simultaneous access to profiles */
    std::mutex m_mutex;
    /** @brief Profile cleanup timer */
    ocpp::helpers::Timer m_cleanup_timer;

    /** @brief Composite schedule period */
    struct Period
    {
        /** @brief Number of seconds since the start */
        time_t start;
        /** @brief Duration in seconds of the period */
        time_t duration;
        /** @brief Setpoint for the period */
        float setpoint;
        /** @brief Unit of the setpoint for the period */
        ocpp::types::ChargingRateUnitType unit;
        /** @brief Number of phases */
        unsigned int nb_phases;
    };

    /** @brief Periodically cleanup expired profiles */
    void cleanupProfiles();

    /** @brief Compute the setpoint of a given connector with a profile list */
    void computeSetpoint(Connector*                                                 connector,
                         ocpp::types::Optional<ocpp::types::SmartChargingSetpoint>& connector_setpoint,
                         ocpp::types::ChargingRateUnitType                          unit,
                         const ProfileDatabase::ChargingProfileList&                profiles_list);

    /** @brief Check if the given profile is active */
    bool isProfileActive(Connector*                          connector,
                         const ocpp::types::ChargingProfile& profile,
                         size_t&                             period,
                         const ocpp::types::DateTime&        time_point);

    /** @brief Fill a setpoint structure with a charging profile and a charging schedule period */
    void fillSetpoint(ocpp::types::SmartChargingSetpoint&        setpoint,
                      ocpp::types::ChargingRateUnitType          unit,
                      const ocpp::types::ChargingProfile&        profile,
                      const ocpp::types::ChargingSchedulePeriod& period);

    /** @brief Convert charging rate units */
    float convertToUnit(float value, ocpp::types::ChargingRateUnitType unit, unsigned int number_phases);

    /** @brief Indicate if a charging profile is valid at a given point in time */
    bool isProfileValid(const ocpp::types::ChargingProfile& profile, const ocpp::types::DateTime& time_point);

    /** @brief Get the kind of charging profile */
    ocpp::types::ChargingProfileKindType getProfileKind(const ocpp::types::ChargingProfile& profile);

    /** @brief Get the start time of a charging profile schedule */
    ocpp::types::DateTime getProfileStartTime(Connector*                           connector,
                                              const ocpp::types::ChargingProfile&  profile,
                                              ocpp::types::ChargingProfileKindType kind,
                                              const ocpp::types::DateTime&         time_point);

    /** @brief Get the composite schedule periods corresponding to a charging profile */
    std::vector<Period> getProfilePeriods(Connector*                          connector,
                                          const ocpp::types::ChargingProfile& profile,
                                          const ocpp::types::DateTime&        time_point,
                                          unsigned int                        duration);

    /** @brief Merge charging profiles periods */
    std::vector<Period> mergeProfilePeriods(const std::vector<Period>& ref_periods, const std::vector<Period>& new_periods);

    /** @brief Merge local limitations periods */
    std::vector<Period> mergeLocalPeriods(const std::vector<Period>& profiles_periods, const std::vector<Period>& local_periods);

    /** @brief Merge the setpoint of a local limitation and a profile limitation */
    void mergeSetpoint(const Period& profile_period, const Period& local_period, Period& merged_period);

    /** @brief Add a local limitation and profiles limitations merged period */
    void addMergedPeriod(const Period& merged_period, std::vector<Period>& periods);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_SMARTCHARGINGMANAGER_H
