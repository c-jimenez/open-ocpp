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

#ifndef OPENOCPP_METERVALUESMANAGER_H
#define OPENOCPP_METERVALUESMANAGER_H

#include "Database.h"
#include "Enums.h"
#include "IConfigManager.h"
#include "IMeterValuesManager.h"
#include "ITriggerMessageManager.h"
#include "Timer.h"

namespace ocpp
{
// Forward declarations
namespace config
{
class IOcppConfig;
} // namespace config
namespace messages
{
class GenericMessageSender;
class IRequestFifo;
} // namespace messages
namespace helpers
{
class WorkerThreadPool;
} // namespace helpers

// Main namespace
namespace chargepoint
{

class Connectors;
class IChargePointEventsHandler;
class IStatusManager;

/** @brief Handle charge point meter values requests */
class MeterValuesManager : public IMeterValuesManager,
                           public ITriggerMessageManager::ITriggerMessageHandler,
                           public ITriggerMessageManager::IExtendedTriggerMessageHandler,
                           public IConfigManager::IConfigChangedListener
{
  public:
    /** @brief Constructor */
    MeterValuesManager(ocpp::config::IOcppConfig&            ocpp_config,
                       ocpp::database::Database&             database,
                       IChargePointEventsHandler&            events_handler,
                       ocpp::helpers::ITimerPool&            timer_pool,
                       ocpp::helpers::WorkerThreadPool&      worker_pool,
                       Connectors&                           connectors,
                       ocpp::messages::GenericMessageSender& msg_sender,
                       ocpp::messages::IRequestFifo&         requests_fifo,
                       IStatusManager&                       status_manager,
                       ITriggerMessageManager&               trigger_manager,
                       IConfigManager&                       config_manager);

    /** @brief Destructor */
    virtual ~MeterValuesManager();

    // IMeterValuesManager interface

    /** @copydoc bool IMeterValuesManager::sendMeterValues(unsigned int, const std::vector<ocpp::types::ocpp16::MeterValue>&) */
    bool sendMeterValues(unsigned int connector_id, const std::vector<ocpp::types::ocpp16::MeterValue>& values) override;

    /** @copydoc void IMeterValuesManager::startSampledMeterValues(unsigned int) */
    void startSampledMeterValues(unsigned int connector_id) override;

    /** @copydoc void IMeterValuesManager::stopSampledMeterValues(unsigned int) */
    void stopSampledMeterValues(unsigned int connector_id) override;

    /** @copydoc void IMeterValuesManager::getTxStopMeterValues(unsigned int, std::vector<ocpp::types::ocpp16::MeterValue>&) */
    void getTxStopMeterValues(unsigned int connector_id, std::vector<ocpp::types::ocpp16::MeterValue>& meter_values) override;

    // ITriggerMessageManager::ITriggerMessageHandler interface

    /** @copydoc bool ITriggerMessageManager::ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTrigger message,
     *                                                                                 const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::ocpp16::MessageTrigger message, const ocpp::types::Optional<unsigned int>& connector_id) override;

    /** @copydoc bool ITriggerMessageManager::ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
     *                                                                                 const ocpp::types::Optional<unsigned int>&) */
    bool onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
                          const ocpp::types::Optional<unsigned int>&  connector_id) override;

    // IConfigChangedListener interface

    /** @copydoc void IConfigChangedListener::configurationValueChanged(const std::string&) */
    void configurationValueChanged(const std::string& key) override;

    /** @brief Name of the clock aligned timer */
    static constexpr const char* CLOCK_ALIGNED_TIMER_NAME = "Clock Aligned";

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;
    /** @brief Worker thread pool */
    ocpp::helpers::WorkerThreadPool& m_worker_pool;
    /** @brief Charge point's connectors */
    Connectors& m_connectors;
    /** @brief Message sender */
    ocpp::messages::GenericMessageSender& m_msg_sender;
    /** @brief Status manager */
    IStatusManager& m_status_manager;
    /** @brief Transaction related requests FIFO */
    ocpp::messages::IRequestFifo& m_requests_fifo;

    /** @brief Clock-aligned meter values timer */
    ocpp::helpers::Timer m_clock_aligned_timer;

    /** @brief Query to look for the meter values associated to a transaction */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to delete the meter values associated to a transaction */
    std::unique_ptr<ocpp::database::Database::Query> m_delete_query;
    /** @brief Query to insert a meter value associated to a transaction */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;

    /** @brief Configure the meter value sample timer */
    void configureMeterValueSampleTimer(const unsigned int connector_id, const std::chrono::seconds interval);
    /** @brief Configure clock-aligned timer */
    void configureClockAlignedTimer(void);
    /** @brief Process clock-aligned meter values */
    void processClockAligned(void);
    /** @brief Process sampled meter values for a given connector */
    void processSampled(unsigned int connector_id);
    /** @brief Process triggered meter values for a given connector */
    void processTriggered(unsigned int connector_id);

    /** @brief Send a meter value request for a given measurand list on a connector */
    void sendMeterValues(
        unsigned int                                                                                                     connector_id,
        const std::vector<std::pair<ocpp::types::ocpp16::Measurand, ocpp::types::Optional<ocpp::types::ocpp16::Phase>>>& measurands,
        ocpp::types::ocpp16::ReadingContext                                                                              context,
        const ocpp::types::Optional<int>& transaction_id = ocpp::types::Optional<int>());

    /** @brief Compute the measurand list from a CSL configuration string */
    std::vector<std::pair<ocpp::types::ocpp16::Measurand, ocpp::types::Optional<ocpp::types::ocpp16::Phase>>> computeMeasurandList(
        const std::string& meter_values, const unsigned int max_count);

    /** @brief Fill a meter value element */
    bool fillMeterValue(
        unsigned int                                                                                                     connector_id,
        const std::vector<std::pair<ocpp::types::ocpp16::Measurand, ocpp::types::Optional<ocpp::types::ocpp16::Phase>>>& measurands,
        ocpp::types::ocpp16::MeterValue&                                                                                 meter_value,
        ocpp::types::ocpp16::ReadingContext                                                                              context);

    /** @brief Initialize the database table */
    void initDatabaseTable();
    /** @brief Serialize a meter value to a string */
    std::string serialize(const ocpp::types::ocpp16::MeterValue& meter_value);
    /** @brief Deserialize a meter value from a string */
    bool deserialize(const std::string& meter_value_str, ocpp::types::ocpp16::MeterValue& meter_value);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_METERVALUESMANAGER_H
