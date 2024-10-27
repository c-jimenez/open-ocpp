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

#include "MeterValuesManager.h"
#include "Connectors.h"
#include "GenericMessageSender.h"
#include "IChargePointEventsHandler.h"
#include "IOcppConfig.h"
#include "IRequestFifo.h"
#include "IStatusManager.h"
#include "Logger.h"
#include "MeterValueConverter.h"
#include "MeterValues.h"
#include "StringHelpers.h"
#include "WorkerThreadPool.h"

#include <functional>

using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

namespace ocpp
{
namespace chargepoint
{
/** @brief Constructor */
MeterValuesManager::MeterValuesManager(ocpp::config::IOcppConfig&            ocpp_config,
                                       ocpp::database::Database&             database,
                                       IChargePointEventsHandler&            events_handler,
                                       ocpp::helpers::ITimerPool&            timer_pool,
                                       ocpp::helpers::WorkerThreadPool&      worker_pool,
                                       Connectors&                           connectors,
                                       ocpp::messages::GenericMessageSender& msg_sender,
                                       ocpp::messages::IRequestFifo&         requests_fifo,
                                       IStatusManager&                       status_manager,
                                       ITriggerMessageManager&               trigger_manager,
                                       IConfigManager&                       config_manager)
    : m_ocpp_config(ocpp_config),
      m_database(database),
      m_events_handler(events_handler),
      m_worker_pool(worker_pool),
      m_connectors(connectors),
      m_msg_sender(msg_sender),
      m_status_manager(status_manager),
      m_requests_fifo(requests_fifo),
      m_clock_aligned_timer(timer_pool, CLOCK_ALIGNED_TIMER_NAME),
      m_find_query(nullptr),
      m_delete_query(nullptr),
      m_insert_query(nullptr)
{
    // Initialize database
    initDatabaseTable();

    // Register messages handlers
    trigger_manager.registerHandler(MessageTrigger::MeterValues, *this);
    trigger_manager.registerHandler(MessageTriggerEnumType::MeterValues, *this);
    m_clock_aligned_timer.setCallback(std::bind(&MeterValuesManager::processClockAligned, this));

    // Register configuration change handler
    config_manager.registerConfigChangedListener("ClockAlignedDataInterval", *this);
    config_manager.registerConfigChangedListener("MeterValueSampleInterval", *this);

    // Start clock aligned and sample timers
    configureClockAlignedTimer();
    for (Connector* connector : m_connectors.getConnectors())
    {
        connector->meter_values_timer.setCallback(std::bind(&MeterValuesManager::processSampled, this, connector->id));
        if (connector->transaction_id != 0)
        {
            startSampledMeterValues(connector->id);
        }
    }
}

/** @brief Destructor */
MeterValuesManager::~MeterValuesManager()
{
    m_clock_aligned_timer.stop();
    for (unsigned int i = 0; i <= m_connectors.getCount(); i++)
    {
        m_connectors.getConnector(i)->meter_values_timer.stop();
    }
}

/** @copydoc bool IMeterValuesManager::sendMeterValues(unsigned int, const std::vector<ocpp::types::ocpp16::MeterValue>&) */
bool MeterValuesManager::sendMeterValues(unsigned int connector_id, const std::vector<ocpp::types::ocpp16::MeterValue>& values)
{
    bool ret = false;

    // Check connector
    if (m_connectors.isValid(connector_id))
    {
        // Prepare request
        MeterValuesReq meter_values_req;
        meter_values_req.connectorId = connector_id;
        meter_values_req.meterValue  = values;
        if (!meter_values_req.meterValue.empty())
        {
            // Send request
            MeterValuesConf meter_values_conf;
            CallResult      res = m_msg_sender.call(METER_VALUES_ACTION, meter_values_req, meter_values_conf);
            ret                 = (res == CallResult::Ok);
        }
    }

    return ret;
}

/** @copydoc void IMeterValuesManager::startSampledMeterValues(unsigned int) */
void MeterValuesManager::startSampledMeterValues(unsigned int connector_id)
{
    // Get interval from configuration
    std::chrono::seconds interval = m_ocpp_config.meterValueSampleInterval();
    if (interval > std::chrono::seconds(0))
    {
        // Get connector
        Connector* connector = m_connectors.getConnector(connector_id);
        if (connector)
        {
            // Start meter value timer for the connector
            connector->meter_values_timer.start(interval);
        }
    }
}

/** @copydoc void IMeterValuesManager::stopSampledMeterValues(unsigned int) */
void MeterValuesManager::stopSampledMeterValues(unsigned int connector_id)
{
    // Get connector
    Connector* connector = m_connectors.getConnector(connector_id);
    if (connector)
    {
        // Stop meter value timer for the connector
        connector->meter_values_timer.stop();
    }
}

/** @copydoc void IMeterValuesManager::getTxStopMeterValues(unsigned int, std::vector<ocpp::types::ocpp16::MeterValue>&) */
void MeterValuesManager::getTxStopMeterValues(unsigned int connector_id, std::vector<ocpp::types::ocpp16::MeterValue>& meter_values)
{
    // Get connector
    Connector* connector = m_connectors.getConnector(connector_id);
    if (connector && m_find_query && m_delete_query)
    {
        // Get data from database
        meter_values.clear();
        m_find_query->bind(0, connector->transaction_id);
        if (m_find_query->exec() && m_find_query->hasRows())
        {
            do
            {
                // Extract table data
                std::string meter_value_str = m_find_query->getString(2);

                // Deserialize meter value
                meter_values.emplace_back();
                MeterValue& meter_value = meter_values.back();
                if (!deserialize(meter_value_str, meter_value))
                {
                    meter_values.pop_back();
                }
            } while (m_find_query->next());
            m_find_query->reset();

            // Clear data from database
            m_delete_query->bind(0, connector->transaction_id);
            m_delete_query->exec();
            m_delete_query->reset();
        }
        m_find_query->reset();
    }
}

/** @copydoc bool ITriggerMessageManager::ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTrigger message, const ocpp::types::Optional<unsigned int>&) */
bool MeterValuesManager::onTriggerMessage(ocpp::types::ocpp16::MessageTrigger        message,
                                          const ocpp::types::Optional<unsigned int>& connector_id)
{
    bool ret = false;
    if (message == MessageTrigger::MeterValues)
    {
        if (connector_id.isSet())
        {
            processTriggered(connector_id);
        }
        else
        {
            for (const Connector* connector : m_connectors.getConnectors())
            {
                unsigned int id = connector->id;
                processTriggered(id);
            }
        }
        ret = true;
    }
    return ret;
}

/** @copydoc bool ITriggerMessageManager::ITriggerMessageHandler::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message, const ocpp::types::Optional<unsigned int>&) */
bool MeterValuesManager::onTriggerMessage(ocpp::types::ocpp16::MessageTriggerEnumType message,
                                          const ocpp::types::Optional<unsigned int>&  connector_id)
{
    bool ret = false;
    if (message == MessageTriggerEnumType::MeterValues)
    {
        if (connector_id.isSet())
        {
            processTriggered(connector_id);
        }
        else
        {
            for (const Connector* connector : m_connectors.getConnectors())
            {
                unsigned int id = connector->id;
                processTriggered(id);
            }
        }
        ret = true;
    }
    return ret;
}

/** @copydoc void IConfigChangedListener::configurationValueChanged(const std::string&) */
void MeterValuesManager::configurationValueChanged(const std::string& key)
{
    if (key == "metervaluesampleinterval")
    {
        // Check new value
        std::chrono::seconds interval = m_ocpp_config.meterValueSampleInterval();
        for (Connector* connector : m_connectors.getConnectors())
        {
            if (interval == std::chrono::seconds(0))
            {
                // Disable meter values
                connector->meter_values_timer.stop();
                LOG_INFO << "Meter values disabled on connector " << connector->id;
            }
            else if (connector->status == ocpp::types::ocpp16::ChargePointStatus::Charging)
            {
                configureMeterValueSampleTimer(connector->id, interval);
            }
        }
    }
    else if (key == "clockaligneddatainterval")
    {
        // Check new value
        std::chrono::seconds interval = m_ocpp_config.clockAlignedDataInterval();
        if (interval == std::chrono::seconds(0))
        {
            // Disable clock aligned values
            m_clock_aligned_timer.stop();

            LOG_INFO << "Clock aligned meter values disabled";
        }
        else
        {
            // Reconfigure clock aligned timer
            configureClockAlignedTimer();
        }
    }
}

/** @brief Configure meter value sample timer */
void MeterValuesManager::configureMeterValueSampleTimer(const unsigned int connector_id, const std::chrono::seconds interval)
{
    Connector* connector = m_connectors.getConnector(connector_id);

    if (!connector)
    {
        return;
    }

    // Reconfigure meter value timer
    // Stop timer
    connector->meter_values_timer.stop();

    LOG_INFO << "Configure meter values on connector " << connector->id << " : interval in seconds = " << interval.count();

    // Compute next due date
    time_t    now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm aligned_time_tm;
#ifdef _MSC_VER
    localtime_s(&aligned_time_tm, &now);
#else  // _MSC_VER
    localtime_r(&now, &aligned_time_tm);
#endif // _MSC_VER
    aligned_time_tm.tm_min = 0;
    aligned_time_tm.tm_sec = 0;
    time_t aligned_time    = std::mktime(&aligned_time_tm);
    while (aligned_time <= now)
    {
        aligned_time += interval.count();
    }
    std::chrono::seconds next_due_interval = std::chrono::seconds(aligned_time - now);
    connector->meter_values_timer.start(std::chrono::milliseconds(next_due_interval));
}

/** @brief Configure clock-aligned timer */
void MeterValuesManager::configureClockAlignedTimer(void)
{
    // Stop timer
    m_clock_aligned_timer.stop();

    // Get interval from configuration
    std::chrono::seconds interval = m_ocpp_config.clockAlignedDataInterval();
    if (interval > std::chrono::seconds(0))
    {
        LOG_INFO << "Configure clock aligned meter values : interval in seconds = " << interval.count();

        // Compute next due date
        time_t    now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm aligned_time_tm;
#ifdef _MSC_VER
        localtime_s(&aligned_time_tm, &now);
#else  // _MSC_VER
        localtime_r(&now, &aligned_time_tm);
#endif // _MSC_VER
        aligned_time_tm.tm_min = 0;
        aligned_time_tm.tm_sec = 0;
        time_t aligned_time    = std::mktime(&aligned_time_tm);
        while (aligned_time <= now)
        {
            aligned_time += interval.count();
        }
        std::chrono::seconds next_due_interval = std::chrono::seconds(aligned_time - now);
        m_clock_aligned_timer.start(std::chrono::milliseconds(next_due_interval));
    }
}

/** @brief Process clock-aligned meter values */
void MeterValuesManager::processClockAligned(void)
{
    // Check if timer interval must be modified (for first due date only)
    std::chrono::seconds timer_interval = m_ocpp_config.clockAlignedDataInterval();
    if (m_clock_aligned_timer.getInterval() != timer_interval)
    {
        m_clock_aligned_timer.restart(timer_interval);
    }

    // Check if charge point has been registered
    if (m_status_manager.getRegistrationStatus() == RegistrationStatus::Accepted)
    {
        // Process in background thread
        m_worker_pool.run<void>(
            [this]
            {
                // Process meter value configuration
                std::string  meter_values        = m_ocpp_config.meterValuesAlignedData();
                unsigned int measurands_max_size = m_ocpp_config.meterValuesAlignedDataMaxLength();
                auto         measurands          = computeMeasurandList(meter_values, measurands_max_size);
                if (!measurands.empty())
                {
                    LOG_DEBUG << "Clock aligned meter values : " << meter_values;

                    // For each connector
                    for (const Connector* connector : m_connectors.getConnectors())
                    {
                        sendMeterValues(connector->id, measurands, ReadingContext::SampleClock);
                    }

                    // Process transaction sampled meter value configuration
                    meter_values        = m_ocpp_config.stopTxnAlignedData();
                    measurands_max_size = m_ocpp_config.stopTxnAlignedDataMaxLength();
                    measurands          = computeMeasurandList(meter_values, measurands_max_size);
                    if (!measurands.empty() && m_insert_query)
                    {
                        LOG_DEBUG << "Clock aligned transaction meter values : " << meter_values;

                        // Fill meter value
                        MeterValue meter_value;
                        for (const Connector* connector : m_connectors.getConnectors())
                        {
                            if ((connector->transaction_id != 0) &&
                                fillMeterValue(connector->id, measurands, meter_value, ReadingContext::SampleClock))
                            {
                                // Serialize value
                                std::string meter_value_str = serialize(meter_value);

                                // Store into database
                                m_insert_query->bind(0u, connector->transaction_id);
                                m_insert_query->bind(1u, meter_value_str);
                                m_insert_query->exec();
                                m_insert_query->reset();
                            }
                        }
                    }
                }
            });
    }
}

/** @brief Process sampled meter values for a given connector */
void MeterValuesManager::processSampled(unsigned int connector_id)
{
    // Process in background thread
    m_worker_pool.run<void>(
        [this, connector_id]
        {
            // Process sampled meter value configuration
            std::string  meter_values        = m_ocpp_config.meterValuesSampledData();
            unsigned int measurands_max_size = m_ocpp_config.meterValuesSampledDataMaxLength();
            auto         measurands          = computeMeasurandList(meter_values, measurands_max_size);
            if (!measurands.empty())
            {
                LOG_DEBUG << "Sampled meter values : " << meter_values;

                // Get connector
                Connector* connector = m_connectors.getConnector(connector_id);
                if (connector)
                {
                    // Send sampled meter values
                    sendMeterValues(connector->id, measurands, ReadingContext::SamplePeriodic, connector->transaction_id);

                    // Process transaction sampled meter value configuration
                    meter_values        = m_ocpp_config.stopTxnSampledData();
                    measurands_max_size = m_ocpp_config.stopTxnSampledDataMaxLength();
                    measurands          = computeMeasurandList(meter_values, measurands_max_size);
                    if (!measurands.empty() && m_insert_query)
                    {
                        LOG_DEBUG << "Sampled transaction meter values : " << meter_values;

                        // Fill meter value
                        MeterValue meter_value;
                        if (fillMeterValue(connector_id, measurands, meter_value, ReadingContext::SamplePeriodic))
                        {
                            // Serialize value
                            std::string meter_value_str = serialize(meter_value);

                            // Store into database
                            m_insert_query->bind(0u, connector->transaction_id);
                            m_insert_query->bind(1u, meter_value_str);
                            m_insert_query->exec();
                            m_insert_query->reset();
                        }
                    }
                }
            }
        });
}

/** @brief Process triggered meter values for a given connector */
void MeterValuesManager::processTriggered(unsigned int connector_id)
{
    // Process in background thread
    m_worker_pool.run<void>(
        [this, connector_id]
        {
            // To let some time for the trigger message reply
            std::this_thread::sleep_for(std::chrono::milliseconds(250u));

            // Process meter value configuration
            std::string  meter_values        = m_ocpp_config.meterValuesSampledData();
            unsigned int measurands_max_size = m_ocpp_config.meterValuesSampledDataMaxLength();
            auto         measurands          = computeMeasurandList(meter_values, measurands_max_size);
            if (!measurands.empty())
            {
                LOG_INFO << "Triggered meter values : " << meter_values;

                // Get connector
                Connector* connector = m_connectors.getConnector(connector_id);
                if (connector)
                {
                    sendMeterValues(connector->id, measurands, ReadingContext::Trigger);
                }
            }
        });
}

/** @brief Send a meter value request for a given measurand list on a connector */
void MeterValuesManager::sendMeterValues(unsigned int                                              connector_id,
                                         const std::vector<std::pair<Measurand, Optional<Phase>>>& measurands,
                                         ocpp::types::ocpp16::ReadingContext                       context,
                                         const ocpp::types::Optional<int>&                         transaction_id)
{
    // Prepare request
    MeterValuesReq meter_values_req;
    meter_values_req.connectorId   = connector_id;
    meter_values_req.transactionId = transaction_id;
    meter_values_req.meterValue.emplace_back();
    MeterValue& meter_value = meter_values_req.meterValue.back();

    // Fill meter value
    if (fillMeterValue(connector_id, measurands, meter_value, context))
    {
        // Don't use FIFO for triggered values
        IRequestFifo* fifo = &m_requests_fifo;
        if (context == ReadingContext::Trigger)
        {
            fifo = nullptr;
        }

        // Send request
        MeterValuesConf meter_values_conf;
        m_msg_sender.call(METER_VALUES_ACTION, meter_values_req, meter_values_conf, fifo, connector_id);
    }
}

/** @brief Compute the measurand list from a CSL configuration string */
std::vector<std::pair<ocpp::types::ocpp16::Measurand, ocpp::types::Optional<ocpp::types::ocpp16::Phase>>> MeterValuesManager::
    computeMeasurandList(const std::string& meter_values, const unsigned int max_count)
{
    std::string trimmed_meter_values(meter_values);
    ocpp::helpers::replace(trimmed_meter_values, " ", "");
    std::vector<std::string> measurands = ocpp::helpers::split(trimmed_meter_values, ',');
    if (measurands.size() > max_count)
    {
        measurands.resize(max_count);
    }
    std::vector<std::pair<Measurand, Optional<Phase>>> measurands_list;
    for (const std::string& measurand_str : measurands)
    {
        bool                     phase_done      = false;
        std::vector<std::string> measurand_split = ocpp::helpers::split(measurand_str, '.');
        if (measurand_split.size() > 1u)
        {
            // Check phase used
            Phase       phase;
            std::string phase_str = measurand_split.back();
            if (PhaseHelper.fromString(phase_str, phase))
            {
                // Convert measurand part
                Measurand   measurand;
                std::string measurand_prefix = measurand_str.substr(0, measurand_str.size() - (phase_str.size() + 1u));
                if (MeasurandHelper.fromString(measurand_prefix, measurand))
                {
                    measurands_list.emplace_back(measurand, phase);
                    phase_done = true;
                }
            }
        }
        if (!phase_done)
        {
            // Phase used not specified
            Measurand measurand;
            if (MeasurandHelper.fromString(measurand_str, measurand))
            {
                measurands_list.emplace_back(measurand, Optional<Phase>());
            }
        }
    }

    return measurands_list;
}

/** @brief Fill a metervalue element */
bool MeterValuesManager::fillMeterValue(
    unsigned int                                                                                                     connector_id,
    const std::vector<std::pair<ocpp::types::ocpp16::Measurand, ocpp::types::Optional<ocpp::types::ocpp16::Phase>>>& measurands,
    ocpp::types::ocpp16::MeterValue&                                                                                 meter_value,
    ocpp::types::ocpp16::ReadingContext                                                                              context)
{
    meter_value.timestamp = DateTime::now();
    meter_value.sampledValue.clear();
    for (const auto& measurand : measurands)
    {
        size_t count = meter_value.sampledValue.size();
        if (!m_events_handler.getMeterValue(connector_id, measurand, meter_value))
        {
            for (size_t i = count; i < meter_value.sampledValue.size(); i++)
            {
                meter_value.sampledValue.pop_back();
            }
        }
        else
        {
            for (size_t i = count; i < meter_value.sampledValue.size(); i++)
            {
                SampledValue& sample_value = meter_value.sampledValue[i];
                sample_value.context       = context;
                sample_value.measurand     = measurand.first;
            }
        }
    }
    return (!meter_value.sampledValue.empty());
}

/** @brief Initialize the database table */
void MeterValuesManager::initDatabaseTable()
{
    // Create database
    auto query = m_database.query("CREATE TABLE IF NOT EXISTS TxMeterValues ("
                                  "[id]	INTEGER,"
                                  "[transaction_id]	INTEGER,"
                                  "[meter_value] VARCHAR(1024),"
                                  "PRIMARY KEY([id] AUTOINCREMENT));");
    if (query)
    {
        if (!query->exec())
        {
            LOG_ERROR << "Could not create transaction meter values table : " << query->lastError();
        }
    }

    // Create parametrized queries
    m_find_query   = m_database.query("SELECT * FROM TxMeterValues WHERE transaction_id=?;");
    m_delete_query = m_database.query("DELETE FROM TxMeterValues WHERE transaction_id=?;");
    m_insert_query = m_database.query("INSERT INTO TxMeterValues VALUES (NULL, ?, ?);");

    // Clear not ongoing transaction data (could happenif connector data has been reset)
    query = m_database.query("SELECT DISTINCT transaction_id FROM TxMeterValues;");
    if (query && query->exec() && query->hasRows())
    {
        do
        {
            bool found          = false;
            int  transaction_id = query->getInt32(0);
            for (const Connector* connector : m_connectors.getConnectors())
            {
                if (connector->transaction_id == transaction_id)
                {
                    found = true;
                    break;
                }
            }
            if (!found && m_delete_query)
            {
                LOG_INFO << "Cleaning meter values associated to not ongoing transaction : " << transaction_id;
                m_delete_query->bind(0, transaction_id);
                m_delete_query->exec();
                m_delete_query->reset();
            }
        } while (query->next());
    }
}

/** @brief Serialize a meter value to a string */
std::string MeterValuesManager::serialize(const ocpp::types::ocpp16::MeterValue& meter_value)
{
    std::string meter_value_str;

    rapidjson::Document meter_value_json(rapidjson::kObjectType);
    MeterValueConverter meter_value_converter;
    meter_value_converter.setAllocator(&meter_value_json.GetAllocator());
    meter_value_converter.toJson(meter_value, meter_value_json);

    rapidjson::StringBuffer                    buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    meter_value_json.Accept(writer);
    meter_value_str = buffer.GetString();
    return meter_value_str;
}

/** @brief Deserialize a meter value from a string */
bool MeterValuesManager::deserialize(const std::string& meter_value_str, ocpp::types::ocpp16::MeterValue& meter_value)
{
    std::string         error_code;
    std::string         error_message;
    rapidjson::Document meter_value_json;
    meter_value_json.Parse(meter_value_str.c_str());
    MeterValueConverter meter_value_converter;
    meter_value_converter.setAllocator(&meter_value_json.GetAllocator());
    return meter_value_converter.fromJson(meter_value_json, meter_value, error_code, error_message);
}
} // namespace chargepoint
} // namespace ocpp
