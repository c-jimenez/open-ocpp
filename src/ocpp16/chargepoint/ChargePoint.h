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

#ifndef OPENOCPP_CHARGEPOINT_H
#define OPENOCPP_CHARGEPOINT_H

#include "Connectors.h"
#include "Database.h"
#include "IChargePoint.h"
#include "IConfigManager.h"
#include "InternalConfigManager.h"
#include "MessagesConverter.h"
#include "MessagesValidator.h"
#include "RequestFifo.h"
#include "RpcClient.h"
#include "SecurityManager.h"
#include "Timer.h"

#include <memory>

namespace ocpp
{
// Forward declarations
namespace messages
{
class MessageDispatcher;
class GenericMessageSender;
} // namespace messages
namespace websockets
{
class IWebsocketClient;
}

// Main namespace
namespace chargepoint
{

class AuthentManager;
class ConfigManager;
class StatusManager;
class TransactionManager;
class TriggerMessageManager;
class ReservationManager;
class DataTransferManager;
class MeterValuesManager;
class SmartChargingManager;
class MaintenanceManager;
class SecurityManager;
class RequestFifoManager;
class Iso15118Manager;

/** @brief Charge point implementation */
class ChargePoint : public IChargePoint,
                    public ocpp::rpc::IRpc::IListener,
                    public ocpp::rpc::IRpc::ISpy,
                    public ocpp::rpc::RpcClient::IListener,
                    public IConfigManager::IConfigChangedListener
{
  public:
    /** @brief Constructor */
    ChargePoint(const ocpp::config::IChargePointConfig&          stack_config,
                ocpp::config::IOcppConfig&                       ocpp_config,
                IChargePointEventsHandler&                       events_handler,
                std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool);

    /** @brief Destructor */
    virtual ~ChargePoint();

    // IChargePoint interface

    /** @copydoc ocpp::helpers::ITimerPool& IChargePoint::getTimerPool() */
    ocpp::helpers::ITimerPool& getTimerPool() override { return *m_timer_pool.get(); }

    /** @copydoc ocpp::helpers::WorkerThreadPool& IChargePoint::getWorkerPool() */
    ocpp::helpers::WorkerThreadPool& getWorkerPool() override { return *m_worker_pool.get(); }

    /** @copydoc bool IChargePoint::resetData() */
    bool resetData() override;

    /** @copydoc bool IChargePoint::resetConnectorData() */
    bool resetConnectorData() override;

    /** @copydoc bool IChargePoint::start() */
    bool start() override;

    /** @copydoc bool IChargePoint::stop() */
    bool stop() override;

    /** @copydoc bool IChargePoint::reconnect() */
    bool reconnect() override;

    /** @copydoc ocpp::types::ocpp16::RegistrationStatus IChargePoint::getRegistrationStatus() */
    ocpp::types::ocpp16::RegistrationStatus getRegistrationStatus() override;

    /** @copydoc ocpp::types::ocpp16::ChargePointStatus IChargePoint::getConnectorStatus(unsigned int) */
    ocpp::types::ocpp16::ChargePointStatus getConnectorStatus(unsigned int connector_id) override;

    /** @copydoc bool IChargePoint::statusNotification(unsigned int,
     *                                                 ocpp::types::ocpp16::ChargePointStatus,
     *                                                 ocpp::types::ocpp16::ChargePointErrorCode,
     *                                                 const std::string&,
     *                                                 const std::string&,
     *                                                 const std::string&) */
    bool statusNotification(unsigned int                              connector_id,
                            ocpp::types::ocpp16::ChargePointStatus    status,
                            ocpp::types::ocpp16::ChargePointErrorCode error_code,
                            const std::string&                        info,
                            const std::string&                        vendor_id,
                            const std::string&                        vendor_error) override;

    /** @copydoc ocpp::types::ocpp16::AuthorizationStatus IChargePoint::authorize(unsigned int, const std::string&, std::string&) */
    ocpp::types::ocpp16::AuthorizationStatus authorize(unsigned int       connector_id,
                                                       const std::string& id_tag,
                                                       std::string&       parent_id) override;

    /** @copydoc ocpp::types::ocpp16::AuthorizationStatus IChargePoint::startTransaction(unsigned int, const std::string&, int&) */
    ocpp::types::ocpp16::AuthorizationStatus startTransaction(unsigned int       connector_id,
                                                              const std::string& id_tag,
                                                              int&               transaction_id) override;

    /** @copydoc bool IChargePoint::stopTransaction(unsigned int, const std::string&, ocpp::types::ocpp16::Reason) */
    bool stopTransaction(unsigned int connector_id, const std::string& id_tag, ocpp::types::ocpp16::Reason reason) override;

    /** @copydoc bool IChargePoint::dataTransfer(const std::string&,
                                                 const std::string&,
                                                 const std::string&,
                                                 ocpp::types::ocpp16::DataTransferStatus&,
                                                 std::string& ) */
    bool dataTransfer(const std::string&                       vendor_id,
                      const std::string&                       message_id,
                      const std::string&                       request_data,
                      ocpp::types::ocpp16::DataTransferStatus& status,
                      std::string&                             response_data) override;

    /** @copydoc bool IChargePoint::sendMeterValues(unsigned int, const std::vector<ocpp::types::ocpp16::MeterValue>&) */
    bool sendMeterValues(unsigned int connector_id, const std::vector<ocpp::types::ocpp16::MeterValue>& values) override;

    /** @copydoc bool IChargePoint::getSetpoint(unsigned int,
                                                ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>&,
                                                ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>&,
                                                ocpp::types::ocpp16::ChargingRateUnitType) */
    bool getSetpoint(unsigned int                                                       connector_id,
                     ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& charge_point_setpoint,
                     ocpp::types::Optional<ocpp::types::ocpp16::SmartChargingSetpoint>& connector_setpoint,
                     ocpp::types::ocpp16::ChargingRateUnitType                          unit) override;

    /** @copydoc bool IChargePoint::notifyFirmwareUpdateStatus(bool) */
    bool notifyFirmwareUpdateStatus(bool success) override;

    // Security extensions

    /** @copydoc bool IChargePoint::logSecurityEvent(const std::string&, const std::string&, bool) */
    bool logSecurityEvent(const std::string& type, const std::string& message, bool critical) override;

    /** @copydoc bool IChargePoint::clearSecurityEvents() */
    bool clearSecurityEvents() override;

    /** @copydoc bool IChargePoint::signCertificate(const ocpp::x509::CertificateRequest&) */
    bool signCertificate(const ocpp::x509::CertificateRequest& csr) override;

    /** @copydoc bool IChargePoint::signCertificate() */
    bool signCertificate() override;

    /** @copydoc bool IChargePoint::notifySignedUpdateFirmwareStatus(ocpp::types::ocpp16::FirmwareStatusEnumType) */
    bool notifySignedUpdateFirmwareStatus(ocpp::types::ocpp16::FirmwareStatusEnumType status) override;

    // ISO 15118 PnC extensions

    /** @copydoc ocpp::types::ocpp16::AuthorizationStatus iso15118Authorize(const ocpp::x509::Certificate&,
                                                                    const std::string&,
                                                                    const std::vector<ocpp::types::ocpp16::OcspRequestDataType>&,
                                                                    ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType>&) */
    ocpp::types::ocpp16::AuthorizationStatus iso15118Authorize(
        const ocpp::x509::Certificate&                                                  certificate,
        const std::string&                                                              id_token,
        const std::vector<ocpp::types::ocpp16::OcspRequestDataType>&                    cert_hash_data,
        ocpp::types::Optional<ocpp::types::ocpp16::AuthorizeCertificateStatusEnumType>& cert_status) override;

    /** @copydoc bool IChargePoint::iso15118GetEVCertificate(const std::string&,
                                                             ocpp::types::ocpp16::CertificateActionEnumType,
                                                             const std::string&,
                                                             std::string&) */
    bool iso15118GetEVCertificate(const std::string&                             iso15118_schema_version,
                                  ocpp::types::ocpp16::CertificateActionEnumType action,
                                  const std::string&                             exi_request,
                                  std::string&                                   exi_response) override;

    /** @copydoc bool IChargePoint::iso15118GetCertificateStatus(const ocpp::types::ocpp16::OcspRequestDataType&, std::string&) */
    bool iso15118GetCertificateStatus(const ocpp::types::ocpp16::OcspRequestDataType& ocsp_request, std::string& ocsp_result) override;

    /** @copydoc bool IChargePoint::iso15118SignCertificate(const ocpp::x509::CertificateRequest&) */
    bool iso15118SignCertificate(const ocpp::x509::CertificateRequest& csr) override;

    // RpcClient::IListener interface

    /** @copydoc void RpcClient::IListener::rpcClientConnected() */
    void rpcClientConnected() override;

    /** @copydoc void RpcClient::IListener::rpcClientFailed() */
    void rpcClientFailed() override;

    // IRpc::IListener interface

    /** @copydoc void IRpc::IListener::rpcDisconnected() */
    void rpcDisconnected() override;

    /** @copydoc void IRpc::IListener::rpcError() */
    void rpcError() override;

    /** @copydoc void IRpc::IListener::rpcCallReceived(const std::string&,
                                                       const rapidjson::Value&,
                                                       rapidjson::Document&,
                                                       std::string&,
                                                       std::string&) */
    bool rpcCallReceived(const std::string&      action,
                         const rapidjson::Value& payload,
                         rapidjson::Document&    response,
                         std::string&            error_code,
                         std::string&            error_message) override;

    /// IRpc::ISpy interface

    /** @copydoc void IRpc::ISpy::rcpMessageReceived(const std::string&) */
    void rcpMessageReceived(const std::string& msg) override;

    /** @copydoc void IRpc::ISpy::rcpMessageSent(const std::string&) */
    void rcpMessageSent(const std::string& msg) override;

    // IConfigChangedListener interface

    /** @copydoc void IConfigChangedListener::configurationValueChanged(const std::string&) */
    void configurationValueChanged(const std::string& key) override;

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief User defined events handler */
    IChargePointEventsHandler& m_events_handler;

    /** @brief Timer pool */
    std::shared_ptr<ocpp::helpers::ITimerPool> m_timer_pool;
    /** @brief Worker thread pool */
    std::shared_ptr<ocpp::helpers::WorkerThreadPool> m_worker_pool;

    /** @brief Database */
    ocpp::database::Database m_database;
    /** @brief Internal configuration manager */
    ocpp::config::InternalConfigManager m_internal_config;

    /** @brief Messages converter */
    ocpp::messages::ocpp16::MessagesConverter m_messages_converter;
    /** @brief Messaes validator */
    ocpp::messages::ocpp16::MessagesValidator m_messages_validator;
    /** @brief Requests FIFO */
    RequestFifo m_requests_fifo;
    /** @brief Security manager */
    SecurityManager m_security_manager;
    /** @brief Indicate that a reconnection process has been scheduled */
    bool m_reconnect_scheduled;

    /** @brief Websocket s*/
    std::unique_ptr<ocpp::websockets::IWebsocketClient> m_ws_client;
    /** @brief RPC client */
    std::unique_ptr<ocpp::rpc::RpcClient> m_rpc_client;
    /** @brief Message dispatcher */
    std::unique_ptr<ocpp::messages::MessageDispatcher> m_msg_dispatcher;
    /** @brief Message sender */
    std::unique_ptr<ocpp::messages::GenericMessageSender> m_msg_sender;

    /** @brief Connectors */
    Connectors m_connectors;

    /** @brief Configuration manager */
    std::unique_ptr<ConfigManager> m_config_manager;
    /** @brief Status manager */
    std::unique_ptr<StatusManager> m_status_manager;
    /** @brief Authentication manager */
    std::unique_ptr<AuthentManager> m_authent_manager;
    /** @brief Transaction manager */
    std::unique_ptr<TransactionManager> m_transaction_manager;
    /** @brief Trigger message manager */
    std::unique_ptr<TriggerMessageManager> m_trigger_manager;
    /** @brief Reservation manager */
    std::unique_ptr<ReservationManager> m_reservation_manager;
    /** @brief Data transfer manager */
    std::unique_ptr<DataTransferManager> m_data_transfer_manager;
    /** @brief Meter values manager */
    std::unique_ptr<MeterValuesManager> m_meter_values_manager;
    /** @brief Smart charging manager */
    std::unique_ptr<SmartChargingManager> m_smart_charging_manager;
    /** @brief Maintenance manager */
    std::unique_ptr<MaintenanceManager> m_maintenance_manager;
    /** @brief Requests FIFO manager */
    std::unique_ptr<RequestFifoManager> m_requests_fifo_manager;
    /** @brief ISO15118 manager */
    std::unique_ptr<Iso15118Manager> m_iso15118_manager;

    /** @brief Uptime timer */
    ocpp::helpers::Timer m_uptime_timer;
    /** @brief Uptime in seconds */
    unsigned int m_uptime;
    /** @brief Disconnected time in seconds */
    unsigned int m_disconnected_time;
    /** @brief Total uptime in seconds */
    unsigned int m_total_uptime;
    /** @brief Total disconnected time in seconds */
    unsigned int m_total_disconnected_time;

    /** @brief Initialize the database */
    void initDatabase();
    /** @brief Process uptime */
    void processUptime();
    /** @brief Save the uptime counter in database */
    void saveUptime();

    /** @brief Schedule a reconnection to the Central System */
    void scheduleReconnect();
    /** @brief Start the connection process to the Central System */
    bool doConnect();

  protected:
    /**
     * @brief Get the standard ocpp config associated to the charge point
     * @return The standard ocpp config associated to the charge point
     */
    ocpp::config::IOcppConfig& getOcppConfig(void) const { return m_ocpp_config; }

    /**
     * @brief Get the events handler associated to the charge point
     * @return The events handler associated to the charge point
     */
    IChargePointEventsHandler& getEventsHandler(void) const { return m_events_handler; }

    /**
     * @brief Get connectors of the charge point
     * @return Connectors of the charge point
     */
    Connectors& getConnectors(void) { return m_connectors; }

    /**
     * @brief Get the configuration manager of the charge point
     * @return The configuration manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    ConfigManager& getConfigManager(void) const;

    /**
     * @brief Get the status manager of the charge point
     * @return The status manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    StatusManager& getStatusManager(void) const;

    /**
     * @brief Get the authentication manager of the charge point
     * @return The authentication manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    AuthentManager& getAuthentManager(void) const;

    /**
     * @brief Get the transaction manager of the charge point
     * @return The transaction manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    TransactionManager& getTransactionManager(void) const;

    /**
     * @brief Get the trigger manager of the charge point
     * @return The trigger manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    TriggerMessageManager& getTriggerManager(void) const;

    /**
     * @brief Get the reservation manager of the charge point
     * @return The reservation manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    ReservationManager& getReservationManager(void) const;

    /**
     * @brief Get the data transfer manager of the charge point
     * @return The data transfer manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    DataTransferManager& getDataTransferManager(void) const;

    /**
     * @brief Get the meter values manager of the charge point
     * @return The meter values manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    MeterValuesManager& getMeterValuesManager(void) const;

    /**
     * @brief Get the smart charging manager of the charge point
     * @return The smart charging manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    SmartChargingManager& getSmartChargingManager(void) const;

    /**
     * @brief Get the maintenance manager of the charge point
     * @return The maintenance manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    MaintenanceManager& getMaintenanceManager(void) const;

    /**
     * @brief Get the requests fifo manager of the charge point
     * @return The requests fifo manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    RequestFifoManager& getRequestsFifoManager(void) const;

    /**
     * @brief Get the iso15118 manager of the charge point
     * @return The iso15118 manager of the charge point
     * @throw std::runtime_error if stack is not started
     */
    Iso15118Manager& getIso15118Manager(void) const;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CHARGEPOINT_H
