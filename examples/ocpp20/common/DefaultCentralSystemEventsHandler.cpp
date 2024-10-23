/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DefaultCentralSystemEventsHandler.h"
#include "PrivateKey.h"
#include "Sha2.h"
#include "StringHelpers.h"

#include <fstream>
#include <iostream>
#include <thread>

// With MSVC compiler, the system() call returns directly the command's return value
#ifdef _MSC_VER
#define WEXITSTATUS(x) (x)
#endif // _MSC_VER

using namespace std;
using namespace ocpp::centralsystem;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;
using namespace ocpp::x509;

/** @brief Display a list of meter values */
static void displayMeterValues(const std::vector<MeterValueType> meter_values);

/** @brief Constructor */
DefaultCentralSystemEventsHandler::DefaultCentralSystemEventsHandler(CentralSystemDemoConfig& config,
                                                                     std::filesystem::path    iso_v2g_root_ca,
                                                                     std::filesystem::path    iso_mo_root_ca,
                                                                     bool                     set_pending_status)
    : m_config(config),
      m_chargepoints_mutex(),
      m_iso_v2g_root_ca(iso_v2g_root_ca),
      m_iso_mo_root_ca(iso_mo_root_ca),
      m_set_pending_status(set_pending_status),
      m_chargepoints(),
      m_pending_chargepoints(),
      m_accepted_chargepoints()
{
}

/** @brief Destructor */
DefaultCentralSystemEventsHandler::~DefaultCentralSystemEventsHandler() { }

// ICentralSystemEventsHandler interface

/** @copydoc bool ICentralSystemEventsHandler::acceptConnection(const char*) */
bool DefaultCentralSystemEventsHandler::acceptConnection(const char* ip_address)
{
    cout << "Accept connection from [" << ip_address << "]" << endl;
    return true;
}

/** @copydoc void ICentralSystemEventsHandler::clientFailedToConnect(const char*) */
void DefaultCentralSystemEventsHandler::clientFailedToConnect(const char* ip_address)
{
    cout << "Client [" << ip_address << "] failed to connect" << endl;
}

/** @copydoc bool ICentralSystemEventsHandler::checkCredentials(const std::string&, const std::string&) */
bool DefaultCentralSystemEventsHandler::checkCredentials(const std::string& chargepoint_id, const std::string& password)
{
    cout << "Check credentials for [" << chargepoint_id << "] : " << password << endl;
    return true;
}

/** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
void DefaultCentralSystemEventsHandler::chargePointConnected(
    std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> chargepoint)
{
    cout << "Charge point [" << chargepoint->identifier() << "] connected" << endl;

    std::lock_guard<std::mutex> lock(m_chargepoints_mutex);

    auto iter_chargepoint = m_chargepoints.find(chargepoint->identifier());
    if (iter_chargepoint == m_chargepoints.end())
    {
        m_chargepoints[chargepoint->identifier()] =
            std::shared_ptr<ChargePointRequestHandler>(new ChargePointRequestHandler(*this, chargepoint));
    }
    else
    {
        cout << "Charge point [" << chargepoint->identifier() << "] already connected" << endl;
        chargepoint.reset();
    }
}

/** @brief Remove a charge point from the connected charge points */
void DefaultCentralSystemEventsHandler::removeChargePoint(const std::string& identifier)
{
    std::thread t(
        [this, identifier = identifier]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
            m_chargepoints.erase(identifier);
            m_pending_chargepoints.erase(identifier);
            m_accepted_chargepoints.erase(identifier);
        });
    t.detach();
}

/** @brief Indicate if a charge point must be accepted */
bool DefaultCentralSystemEventsHandler::isAcceptedChargePoint(const std::string& identifier)
{
    std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
    return (m_accepted_chargepoints.find(identifier) != m_accepted_chargepoints.end());
}

/** @brief Add a charge point to the pending list */
void DefaultCentralSystemEventsHandler::addPendingChargePoint(
    std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> chargepoint)
{
    std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
    m_pending_chargepoints[chargepoint->identifier()] = chargepoint;
}

/** @brief Add a charge point to the accepted list */
void DefaultCentralSystemEventsHandler::addAcceptedChargePoint(
    std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20> chargepoint)
{
    std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
    m_accepted_chargepoints[chargepoint->identifier()] = chargepoint;
}

/** @brief Constructor */
DefaultCentralSystemEventsHandler::ChargePointRequestHandler::ChargePointRequestHandler(
    DefaultCentralSystemEventsHandler&                                              event_handler,
    std::shared_ptr<ocpp::centralsystem::ocpp20::ICentralSystem20::IChargePoint20>& chargepoint)
    : m_event_handler(event_handler), m_chargepoint(chargepoint)
{
    m_chargepoint->registerHandler(*this);
}

/** @brief Destructor */
DefaultCentralSystemEventsHandler::ChargePointRequestHandler::~ChargePointRequestHandler() { }

// IChargePointRequestHandler20 interface

/** @brief Called to notify the disconnection of the charge point */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::disconnected()
{
    cout << "[" << m_chargepoint->identifier() << "] - Disconnected" << endl;
    m_event_handler.removeChargePoint(m_chargepoint->identifier());
}

/** @brief Called on reception of a BootNotification request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onBootNotification(
    const ocpp::messages::ocpp20::BootNotificationReq& request,
    ocpp::messages::ocpp20::BootNotificationConf&      response,
    std::string&                                       error,
    std::string&                                       message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - BootNotification : vendor = " << request.chargingStation.vendorName.str()
         << " model = " << request.chargingStation.model.str() << " S/N = " << request.chargingStation.serialNumber.value().str()
         << " version = " << request.chargingStation.firmwareVersion.value().str() << endl;

    response.currentTime = DateTime::now();
    response.interval    = m_event_handler.getConfig().heartbeatInterval().count();
    response.status      = RegistrationStatusEnumType::Accepted;
    if (m_event_handler.setPendingEnabled())
    {
        if (!m_event_handler.isAcceptedChargePoint(m_chargepoint->identifier()))
        {
            m_event_handler.addPendingChargePoint(m_chargepoint);
            response.interval = m_event_handler.getConfig().bootNotificationRetryInterval().count();
            response.status   = RegistrationStatusEnumType::Pending;
        }
    }

    return ret;
}

/** @brief Called on reception of a Authorize request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onAuthorize(const ocpp::messages::ocpp20::AuthorizeReq& request,
                                                                               ocpp::messages::ocpp20::AuthorizeConf&      response,
                                                                               std::string&                                error,
                                                                               std::string&                                message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - Authorize"
         << " type = " << IdTokenEnumTypeHelper.toString(request.idToken.type) << " token = " << request.idToken.idToken.str() << endl;

    response.idTokenInfo.status                      = AuthorizationStatusEnumType::Accepted;
    response.idTokenInfo.cacheExpiryDateTime.value() = DateTime(DateTime::now().timestamp() + 3600);

    return ret;
}

/** @brief Called on reception of a ClearedChargingLimit request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onClearedChargingLimit(
    const ocpp::messages::ocpp20::ClearedChargingLimitReq& request,
    ocpp::messages::ocpp20::ClearedChargingLimitConf&      response,
    std::string&                                           error,
    std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - ClearedChargingLimit" << endl;

    return ret;
}

/** @brief Called on reception of a CostUpdated request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onCostUpdated(const ocpp::messages::ocpp20::CostUpdatedReq& request,
                                                                                 ocpp::messages::ocpp20::CostUpdatedConf&      response,
                                                                                 std::string&                                  error,
                                                                                 std::string&                                  message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - CostUpdated" << endl;

    return ret;
}

/** @brief Called on reception of a DataTransfer request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onDataTransfer(const ocpp::messages::ocpp20::DataTransferReq& request,
                                                                                  ocpp::messages::ocpp20::DataTransferConf&      response,
                                                                                  std::string&                                   error,
                                                                                  std::string&                                   message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - DataTransfer" << endl;

    response.status = DataTransferStatusEnumType::UnknownVendorId;

    return ret;
}

/** @brief Called on reception of a FirmwareStatusNotification request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onFirmwareStatusNotification(
    const ocpp::messages::ocpp20::FirmwareStatusNotificationReq& request,
    ocpp::messages::ocpp20::FirmwareStatusNotificationConf&      response,
    std::string&                                                 error,
    std::string&                                                 message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - FirmwareStatusNotification" << endl;

    return ret;
}

/** @brief Called on reception of a Heartbeat request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onHeartbeat(const ocpp::messages::ocpp20::HeartbeatReq& request,
                                                                               ocpp::messages::ocpp20::HeartbeatConf&      response,
                                                                               std::string&                                error,
                                                                               std::string&                                message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - Heartbeat" << endl;

    response.currentTime = DateTime::now();

    return ret;
}

/** @brief Called on reception of a LogStatusNotification request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onLogStatusNotification(
    const ocpp::messages::ocpp20::LogStatusNotificationReq& request,
    ocpp::messages::ocpp20::LogStatusNotificationConf&      response,
    std::string&                                            error,
    std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - LogStatusNotification" << endl;

    return ret;
}

/** @brief Called on reception of a MeterValues request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onMeterValues(const ocpp::messages::ocpp20::MeterValuesReq& request,
                                                                                 ocpp::messages::ocpp20::MeterValuesConf&      response,
                                                                                 std::string&                                  error,
                                                                                 std::string&                                  message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - MeterValues" << endl;
    displayMeterValues(request.meterValue);

    return ret;
}

/** @brief Called on reception of a NotifyChargingLimit request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyChargingLimit(
    const ocpp::messages::ocpp20::NotifyChargingLimitReq& request,
    ocpp::messages::ocpp20::NotifyChargingLimitConf&      response,
    std::string&                                          error,
    std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyChargingLimit" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyCustomerInformation request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyCustomerInformation(
    const ocpp::messages::ocpp20::NotifyCustomerInformationReq& request,
    ocpp::messages::ocpp20::NotifyCustomerInformationConf&      response,
    std::string&                                                error,
    std::string&                                                message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyCustomerInformation" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyDisplayMessages request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyDisplayMessages(
    const ocpp::messages::ocpp20::NotifyDisplayMessagesReq& request,
    ocpp::messages::ocpp20::NotifyDisplayMessagesConf&      response,
    std::string&                                            error,
    std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyDisplayMessages" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyEVChargingNeeds request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyEVChargingNeeds(
    const ocpp::messages::ocpp20::NotifyEVChargingNeedsReq& request,
    ocpp::messages::ocpp20::NotifyEVChargingNeedsConf&      response,
    std::string&                                            error,
    std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyEVChargingNeeds" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyEVChargingSchedule request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyEVChargingSchedule(
    const ocpp::messages::ocpp20::NotifyEVChargingScheduleReq& request,
    ocpp::messages::ocpp20::NotifyEVChargingScheduleConf&      response,
    std::string&                                               error,
    std::string&                                               message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyEVChargingSchedule" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyEvent request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyEvent(const ocpp::messages::ocpp20::NotifyEventReq& request,
                                                                                 ocpp::messages::ocpp20::NotifyEventConf&      response,
                                                                                 std::string&                                  error,
                                                                                 std::string&                                  message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyEvent" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyMonitoringReport request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyMonitoringReport(
    const ocpp::messages::ocpp20::NotifyMonitoringReportReq& request,
    ocpp::messages::ocpp20::NotifyMonitoringReportConf&      response,
    std::string&                                             error,
    std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyMonitoringReport" << endl;

    return ret;
}

/** @brief Called on reception of a NotifyReport request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onNotifyReport(const ocpp::messages::ocpp20::NotifyReportReq& request,
                                                                                  ocpp::messages::ocpp20::NotifyReportConf&      response,
                                                                                  std::string&                                   error,
                                                                                  std::string&                                   message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - NotifyReport" << endl;

    return ret;
}

/** @brief Called on reception of a PublishFirmwareStatusNotification request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onPublishFirmwareStatusNotification(
    const ocpp::messages::ocpp20::PublishFirmwareStatusNotificationReq& request,
    ocpp::messages::ocpp20::PublishFirmwareStatusNotificationConf&      response,
    std::string&                                                        error,
    std::string&                                                        message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - PublishFirmwareStatusNotification" << endl;

    return ret;
}

/** @brief Called on reception of a ReportChargingProfiles request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onReportChargingProfiles(
    const ocpp::messages::ocpp20::ReportChargingProfilesReq& request,
    ocpp::messages::ocpp20::ReportChargingProfilesConf&      response,
    std::string&                                             error,
    std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - ReportChargingProfiles" << endl;

    return ret;
}

/** @brief Called on reception of a ReservationStatusUpdate request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onReservationStatusUpdate(
    const ocpp::messages::ocpp20::ReservationStatusUpdateReq& request,
    ocpp::messages::ocpp20::ReservationStatusUpdateConf&      response,
    std::string&                                              error,
    std::string&                                              message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - ReservationStatusUpdate" << endl;

    return ret;
}

/** @brief Called on reception of a SecurityEventNotification request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onSecurityEventNotification(
    const ocpp::messages::ocpp20::SecurityEventNotificationReq& request,
    ocpp::messages::ocpp20::SecurityEventNotificationConf&      response,
    std::string&                                                error,
    std::string&                                                message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - SecurityEventNotification" << endl;

    return ret;
}

/** @brief Called on reception of a SignCertificate request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onSignCertificate(
    const ocpp::messages::ocpp20::SignCertificateReq& request,
    ocpp::messages::ocpp20::SignCertificateConf&      response,
    std::string&                                      error,
    std::string&                                      message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - SignCertificate" << endl;

    response.status = GenericStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a StatusNotification request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onStatusNotification(
    const ocpp::messages::ocpp20::StatusNotificationReq& request,
    ocpp::messages::ocpp20::StatusNotificationConf&      response,
    std::string&                                         error,
    std::string&                                         message)
{
    bool ret = true;

    (void)request;
    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - StatusNotification" << endl;

    return ret;
}

/** @brief Called on reception of a TransactionEvent request from the charge point */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::onTransactionEvent(
    const ocpp::messages::ocpp20::TransactionEventReq& request,
    ocpp::messages::ocpp20::TransactionEventConf&      response,
    std::string&                                       error,
    std::string&                                       message)
{
    bool ret = true;

    (void)response;
    (void)error;
    (void)message;

    cout << "[" << m_chargepoint->identifier() << "] - TransactionEvent" << endl;

    switch (request.eventType)
    {
        case TransactionEventEnumType::Started:
        {
            cout << "Start transaction : id = " << request.transactionInfo.transactionId.str() << endl;
        }
        break;

        case TransactionEventEnumType::Ended:
        {
            cout << "End transaction : id = " << request.transactionInfo.transactionId.str() << endl;
        }
        break;

        case TransactionEventEnumType::Updated:
        {
            cout << "Transaction update : id = " << request.transactionInfo.transactionId.str() << endl;
            displayMeterValues(request.meterValue);
        }
        break;

        default:
            break;
    }

    return ret;
}

/** @brief Display a list of meter values */
static void displayMeterValues(const std::vector<MeterValueType> meter_values)
{
    for (const MeterValueType& meter_value : meter_values)
    {
        cout << " - timestamp : " << meter_value.timestamp.str() << ", sampled values : " << endl;
        for (const SampledValueType& sampled_value : meter_value.sampledValue)
        {
            cout << "    - value = " << sampled_value.value;
            if (sampled_value.unitOfMeasure.isSet())
            {
                cout << ", unit = " << sampled_value.unitOfMeasure.value().unit.value().str();
            }
            if (sampled_value.phase.isSet())
            {
                cout << ", phase = " << PhaseEnumTypeHelper.toString(sampled_value.phase);
            }
            if (sampled_value.measurand.isSet())
            {
                cout << ", measurand = " << MeasurandEnumTypeHelper.toString(sampled_value.measurand);
            }
            if (sampled_value.context.isSet())
            {
                cout << ", context = " << ReadingContextEnumTypeHelper.toString(sampled_value.context);
            }
            if (sampled_value.location.isSet())
            {
                cout << ", location = " << LocationEnumTypeHelper.toString(sampled_value.location);
            }
            cout << endl;
        }
    }
}
