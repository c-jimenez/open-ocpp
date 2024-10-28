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

#include "DefaultChargePointEventsHandler.h"
#include "ChargePointDemoConfig.h"
#include "NotifyReport20.h"

#include <fstream>
#include <iostream>
#include <thread>

// With MSVC compiler, the system() call returns directly the command's return value
#ifdef _MSC_VER
#define WEXITSTATUS(x) (x)
#endif // _MSC_VER

using namespace std;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp20;

/** @brief Constructor */
DefaultChargePointEventsHandler::DefaultChargePointEventsHandler(ChargePointDemoConfig&                     config,
                                                                 ocpp::chargepoint::ocpp20::IDeviceModel20& device_model,
                                                                 const std::filesystem::path&               working_dir)
    : m_config(config), m_device_model(device_model), m_chargepoint(nullptr), m_working_dir(working_dir), m_is_connected(false)
{
    m_device_model.registerListener(*this);
}

/** @brief Destructor */
DefaultChargePointEventsHandler::~DefaultChargePointEventsHandler() { }

/** @copydoc void IChargePointEventsHandler20::connectionStateChanged() */
void DefaultChargePointEventsHandler::connectionFailed()
{
    cout << "Connection failed" << endl;
}

/** @copydoc void IChargePointEventsHandler20::connectionStateChanged(bool) */
void DefaultChargePointEventsHandler::connectionStateChanged(bool isConnected)
{
    cout << "Connection state changed : " << isConnected << endl;
    m_is_connected = isConnected;
}

// IDeviceModel20 interface

/** @brief Called to retrieve the value of a variable */
void DefaultChargePointEventsHandler::getVariable(ocpp::types::ocpp20::GetVariableResultType& var)
{
    std::string value;
    m_config.getDeviceModelValue(var.component, var.variable, value);
    var.attributeValue.value().assign(std::move(value));
}

/** @brief Called to set the value of a variable */
ocpp::types::ocpp20::SetVariableStatusEnumType DefaultChargePointEventsHandler::setVariable(
    const ocpp::types::ocpp20::SetVariableDataType& var)
{
    m_config.setDeviceModelValue(var.component, var.variable, var.attributeValue.str());
    return SetVariableStatusEnumType::Accepted;
}

// OCPP operations

/** @brief Called on reception of a CancelReservation message from the central */
bool DefaultChargePointEventsHandler::onCancelReservation(const ocpp::messages::ocpp20::CancelReservationReq& request,
                                                          ocpp::messages::ocpp20::CancelReservationConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "CancelReservation" << endl;

    response.status = CancelReservationStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a CertificateSigned message from the central */
bool DefaultChargePointEventsHandler::onCertificateSigned(const ocpp::messages::ocpp20::CertificateSignedReq& request,
                                                          ocpp::messages::ocpp20::CertificateSignedConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "CertificateSigned" << endl;

    response.status = CertificateSignedStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a ChangeAvailability message from the central */
bool DefaultChargePointEventsHandler::onChangeAvailability(const ocpp::messages::ocpp20::ChangeAvailabilityReq& request,
                                                           ocpp::messages::ocpp20::ChangeAvailabilityConf&      response,
                                                           std::string&                                         error,
                                                           std::string&                                         message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ChangeAvailability" << endl;

    response.status = ChangeAvailabilityStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a ClearCache message from the central */
bool DefaultChargePointEventsHandler::onClearCache(const ocpp::messages::ocpp20::ClearCacheReq& request,
                                                   ocpp::messages::ocpp20::ClearCacheConf&      response,
                                                   std::string&                                 error,
                                                   std::string&                                 message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ClearCache" << endl;

    response.status = ClearCacheStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a ClearChargingProfile message from the central */
bool DefaultChargePointEventsHandler::onClearChargingProfile(const ocpp::messages::ocpp20::ClearChargingProfileReq& request,
                                                             ocpp::messages::ocpp20::ClearChargingProfileConf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ClearChargingProfile" << endl;

    response.status = ClearChargingProfileStatusEnumType::Unknown;

    return ret;
}

/** @brief Called on reception of a ClearDisplayMessage message from the central */
bool DefaultChargePointEventsHandler::onClearDisplayMessage(const ocpp::messages::ocpp20::ClearDisplayMessageReq& request,
                                                            ocpp::messages::ocpp20::ClearDisplayMessageConf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ClearDisplayMessage" << endl;

    response.status = ClearMessageStatusEnumType::Unknown;

    return ret;
}

/** @brief Called on reception of a ClearVariableMonitoring message from the central */
bool DefaultChargePointEventsHandler::onClearVariableMonitoring(const ocpp::messages::ocpp20::ClearVariableMonitoringReq& request,
                                                                ocpp::messages::ocpp20::ClearVariableMonitoringConf&      response,
                                                                std::string&                                              error,
                                                                std::string&                                              message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "ClearVariableMonitoring" << endl;

    for (const auto& id : request.id)
    {
        (void)id;

        ClearMonitoringResultType result;
        result.status = ClearMonitoringStatusEnumType::Rejected;
        response.clearMonitoringResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a CustomerInformation message from the central */
bool DefaultChargePointEventsHandler::onCustomerInformation(const ocpp::messages::ocpp20::CustomerInformationReq& request,
                                                            ocpp::messages::ocpp20::CustomerInformationConf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "CustomerInformation" << endl;

    response.status = CustomerInformationStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a DataTransfer message from the central */
bool DefaultChargePointEventsHandler::onDataTransfer(const ocpp::messages::ocpp20::DataTransferReq& request,
                                                     ocpp::messages::ocpp20::DataTransferConf&      response,
                                                     std::string&                                   error,
                                                     std::string&                                   message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "DataTransfer" << endl;

    response.status = DataTransferStatusEnumType::UnknownVendorId;

    return ret;
}

/** @brief Called on reception of a DeleteCertificate message from the central */
bool DefaultChargePointEventsHandler::onDeleteCertificate(const ocpp::messages::ocpp20::DeleteCertificateReq& request,
                                                          ocpp::messages::ocpp20::DeleteCertificateConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "DeleteCertificate" << endl;

    response.status = DeleteCertificateStatusEnumType::NotFound;

    return ret;
}

/** @brief Called on reception of a Get15118EVCertificate message from the central */
bool DefaultChargePointEventsHandler::onGet15118EVCertificate(const ocpp::messages::ocpp20::Get15118EVCertificateReq& request,
                                                              ocpp::messages::ocpp20::Get15118EVCertificateConf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "Get15118EVCertificate" << endl;

    response.status = Iso15118EVCertificateStatusEnumType::Failed;

    return ret;
}

/** @brief Called on reception of a GetBaseReport message from the central */
bool DefaultChargePointEventsHandler::onGetBaseReport(const ocpp::messages::ocpp20::GetBaseReportReq& request,
                                                      ocpp::messages::ocpp20::GetBaseReportConf&      response,
                                                      std::string&                                    error,
                                                      std::string&                                    message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "GetBaseReport" << endl;

    if (request.reportBase == ReportBaseEnumType::SummaryInventory)
    {
        response.status = GenericDeviceModelStatusEnumType::NotSupported;
    }
    else
    {
        std::thread report_thread(
            [this, type = request.reportBase, req_id = request.requestId]
            {
                NotifyReportReq notif_req;
                notif_req.requestId   = req_id;
                notif_req.generatedAt = DateTime::now();
                notif_req.seqNo       = 0;
                notif_req.tbc         = false;

                const auto& device_model = m_device_model.getModel();
                for (const auto& [_, comps] : device_model.components)
                {
                    for (const auto& component : comps)
                    {
                        for (const auto& [_, vars] : component.variables)
                        {
                            for (const auto& [_, var] : vars)
                            {
                                if ((type == ReportBaseEnumType::FullInventory) || !var.attributes.type.isSet() ||
                                    (var.attributes.mutability.value() == MutabilityEnumType::ReadWrite))
                                {
                                    ReportDataType report_data;
                                    report_data.component.name.assign(component.name);
                                    if (component.instance.isSet())
                                    {
                                        report_data.component.instance.value().assign(component.instance.value());
                                    }
                                    if (component.evse.isSet())
                                    {
                                        report_data.component.evse.value().id = component.evse.value();
                                    }
                                    if (component.connector.isSet())
                                    {
                                        report_data.component.evse.value().connectorId.value() = component.connector.value();
                                    }
                                    report_data.variable.name.assign(var.name);
                                    if (var.instance.isSet())
                                    {
                                        report_data.variable.instance.value().assign(var.instance.value());
                                    }
                                    report_data.variableAttribute.push_back(var.attributes);
                                    report_data.variableCharacteristics = var.characteristics;

                                    notif_req.reportData.push_back(std::move(report_data));
                                }
                            }
                        }
                    }
                }

                (void)device_model;

                std::this_thread::sleep_for(std::chrono::seconds(1));

                NotifyReportConf notif_conf;
                std::string      error;
                std::string      error_msg;
                m_chargepoint->call(notif_req, notif_conf, error, error_msg);
            });
        report_thread.detach();
    }

    return ret;
}

/** @brief Called on reception of a GetCertificateStatus message from the central */
bool DefaultChargePointEventsHandler::onGetCertificateStatus(const ocpp::messages::ocpp20::GetCertificateStatusReq& request,
                                                             ocpp::messages::ocpp20::GetCertificateStatusConf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetCertificateStatus" << endl;

    response.status = GetCertificateStatusEnumType::Failed;

    return ret;
}

/** @brief Called on reception of a GetChargingProfiles message from the central */
bool DefaultChargePointEventsHandler::onGetChargingProfiles(const ocpp::messages::ocpp20::GetChargingProfilesReq& request,
                                                            ocpp::messages::ocpp20::GetChargingProfilesConf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetChargingProfiles" << endl;

    response.status = GetChargingProfileStatusEnumType::NoProfiles;

    return ret;
}

/** @brief Called on reception of a GetCompositeSchedule message from the central */
bool DefaultChargePointEventsHandler::onGetCompositeSchedule(const ocpp::messages::ocpp20::GetCompositeScheduleReq& request,
                                                             ocpp::messages::ocpp20::GetCompositeScheduleConf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetCompositeSchedule" << endl;

    response.status = GenericStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a GetDisplayMessages message from the central */
bool DefaultChargePointEventsHandler::onGetDisplayMessages(const ocpp::messages::ocpp20::GetDisplayMessagesReq& request,
                                                           ocpp::messages::ocpp20::GetDisplayMessagesConf&      response,
                                                           std::string&                                         error,
                                                           std::string&                                         message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetDisplayMessages" << endl;

    response.status = GetDisplayMessagesStatusEnumType::Unknown;

    return ret;
}

/** @brief Called on reception of a GetInstalledCertificateIds message from the central */
bool DefaultChargePointEventsHandler::onGetInstalledCertificateIds(const ocpp::messages::ocpp20::GetInstalledCertificateIdsReq& request,
                                                                   ocpp::messages::ocpp20::GetInstalledCertificateIdsConf&      response,
                                                                   std::string&                                                 error,
                                                                   std::string&                                                 message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetInstalledCertificateIds" << endl;

    response.status = GetInstalledCertificateStatusEnumType::NotFound;

    return ret;
}

/** @brief Called on reception of a GetLocalListVersion message from the central */
bool DefaultChargePointEventsHandler::onGetLocalListVersion(const ocpp::messages::ocpp20::GetLocalListVersionReq& request,
                                                            ocpp::messages::ocpp20::GetLocalListVersionConf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetLocalListVersion" << endl;

    response.versionNumber = 0;

    return ret;
}

/** @brief Called on reception of a GetLog message from the central */
bool DefaultChargePointEventsHandler::onGetLog(const ocpp::messages::ocpp20::GetLogReq& request,
                                               ocpp::messages::ocpp20::GetLogConf&      response,
                                               std::string&                             error,
                                               std::string&                             message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetLog" << endl;

    response.status = LogStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a GetMonitoringReport message from the central */
bool DefaultChargePointEventsHandler::onGetMonitoringReport(const ocpp::messages::ocpp20::GetMonitoringReportReq& request,
                                                            ocpp::messages::ocpp20::GetMonitoringReportConf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetMonitoringReport" << endl;

    response.status = GenericDeviceModelStatusEnumType::NotSupported;

    return ret;
}

/** @brief Called on reception of a GetReport message from the central */
bool DefaultChargePointEventsHandler::onGetReport(const ocpp::messages::ocpp20::GetReportReq& request,
                                                  ocpp::messages::ocpp20::GetReportConf&      response,
                                                  std::string&                                error,
                                                  std::string&                                message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetReport" << endl;

    response.status = GenericDeviceModelStatusEnumType::NotSupported;

    return ret;
}

/** @brief Called on reception of a GetTransactionStatus message from the central */
bool DefaultChargePointEventsHandler::onGetTransactionStatus(const ocpp::messages::ocpp20::GetTransactionStatusReq& request,
                                                             ocpp::messages::ocpp20::GetTransactionStatusConf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetTransactionStatus" << endl;

    response.messagesInQueue = false;

    return ret;
}

/** @brief Called on reception of a GetVariables message from the central */
bool DefaultChargePointEventsHandler::onGetVariables(const ocpp::messages::ocpp20::GetVariablesReq& request,
                                                     ocpp::messages::ocpp20::GetVariablesConf&      response,
                                                     std::string&                                   error,
                                                     std::string&                                   message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "GetVariables" << endl;

    for (const auto& var_data : request.getVariableData)
    {
        GetVariableResultType result = m_device_model.getVariable(var_data);
        response.getVariableResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a InstallCertificate message from the central */
bool DefaultChargePointEventsHandler::onInstallCertificate(const ocpp::messages::ocpp20::InstallCertificateReq& request,
                                                           ocpp::messages::ocpp20::InstallCertificateConf&      response,
                                                           std::string&                                         error,
                                                           std::string&                                         message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "InstallCertificate" << endl;

    response.status = InstallCertificateStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a PublishFirmware message from the central */
bool DefaultChargePointEventsHandler::onPublishFirmware(const ocpp::messages::ocpp20::PublishFirmwareReq& request,
                                                        ocpp::messages::ocpp20::PublishFirmwareConf&      response,
                                                        std::string&                                      error,
                                                        std::string&                                      message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "PublishFirmware" << endl;

    response.status = GenericStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a RequestStartTransaction message from the central */
bool DefaultChargePointEventsHandler::onRequestStartTransaction(const ocpp::messages::ocpp20::RequestStartTransactionReq& request,
                                                                ocpp::messages::ocpp20::RequestStartTransactionConf&      response,
                                                                std::string&                                              error,
                                                                std::string&                                              message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "RequestStartTransaction" << endl;

    response.status = RequestStartStopStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a RequestStopTransaction message from the central */
bool DefaultChargePointEventsHandler::onRequestStopTransaction(const ocpp::messages::ocpp20::RequestStopTransactionReq& request,
                                                               ocpp::messages::ocpp20::RequestStopTransactionConf&      response,
                                                               std::string&                                             error,
                                                               std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "RequestStopTransaction" << endl;

    response.status = RequestStartStopStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a ReserveNow message from the central */
bool DefaultChargePointEventsHandler::onReserveNow(const ocpp::messages::ocpp20::ReserveNowReq& request,
                                                   ocpp::messages::ocpp20::ReserveNowConf&      response,
                                                   std::string&                                 error,
                                                   std::string&                                 message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ReserveNow" << endl;

    response.status = ReserveNowStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a Reset message from the central */
bool DefaultChargePointEventsHandler::onReset(const ocpp::messages::ocpp20::ResetReq& request,
                                              ocpp::messages::ocpp20::ResetConf&      response,
                                              std::string&                            error,
                                              std::string&                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "Reset" << endl;

    response.status = ResetStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a SendLocalList message from the central */
bool DefaultChargePointEventsHandler::onSendLocalList(const ocpp::messages::ocpp20::SendLocalListReq& request,
                                                      ocpp::messages::ocpp20::SendLocalListConf&      response,
                                                      std::string&                                    error,
                                                      std::string&                                    message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SendLocalList" << endl;

    response.status = SendLocalListStatusEnumType::Failed;

    return ret;
}

/** @brief Called on reception of a SetChargingProfile message from the central */
bool DefaultChargePointEventsHandler::onSetChargingProfile(const ocpp::messages::ocpp20::SetChargingProfileReq& request,
                                                           ocpp::messages::ocpp20::SetChargingProfileConf&      response,
                                                           std::string&                                         error,
                                                           std::string&                                         message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetChargingProfile" << endl;

    response.status = ChargingProfileStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a SetDisplayMessage message from the central */
bool DefaultChargePointEventsHandler::onSetDisplayMessage(const ocpp::messages::ocpp20::SetDisplayMessageReq& request,
                                                          ocpp::messages::ocpp20::SetDisplayMessageConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetDisplayMessage" << endl;

    response.status = DisplayMessageStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a SetMonitoringBase message from the central */
bool DefaultChargePointEventsHandler::onSetMonitoringBase(const ocpp::messages::ocpp20::SetMonitoringBaseReq& request,
                                                          ocpp::messages::ocpp20::SetMonitoringBaseConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetMonitoringBase" << endl;

    response.status = GenericDeviceModelStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a SetMonitoringLevel message from the central */
bool DefaultChargePointEventsHandler::onSetMonitoringLevel(const ocpp::messages::ocpp20::SetMonitoringLevelReq& request,
                                                           ocpp::messages::ocpp20::SetMonitoringLevelConf&      response,
                                                           std::string&                                         error,
                                                           std::string&                                         message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetMonitoringLevel" << endl;

    response.status = GenericStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a SetNetworkProfile message from the central */
bool DefaultChargePointEventsHandler::onSetNetworkProfile(const ocpp::messages::ocpp20::SetNetworkProfileReq& request,
                                                          ocpp::messages::ocpp20::SetNetworkProfileConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetNetworkProfile" << endl;

    response.status = SetNetworkProfileStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a SetVariableMonitoring message from the central */
bool DefaultChargePointEventsHandler::onSetVariableMonitoring(const ocpp::messages::ocpp20::SetVariableMonitoringReq& request,
                                                              ocpp::messages::ocpp20::SetVariableMonitoringConf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "SetVariableMonitoring" << endl;

    for (const auto& data : request.setMonitoringData)
    {
        (void)data;

        SetMonitoringResultType result;
        result.status    = SetMonitoringStatusEnumType::Rejected;
        result.type      = data.type;
        result.component = data.component;
        result.variable  = data.variable;
        result.severity  = data.severity;
        response.setMonitoringResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a SetVariables message from the central */
bool DefaultChargePointEventsHandler::onSetVariables(const ocpp::messages::ocpp20::SetVariablesReq& request,
                                                     ocpp::messages::ocpp20::SetVariablesConf&      response,
                                                     std::string&                                   error,
                                                     std::string&                                   message)
{
    bool ret = true;

    (void)response;
    (void)error;
    (void)message;

    cout << "SetVariables" << endl;

    for (const auto& var_data : request.setVariableData)
    {
        SetVariableResultType result = m_device_model.setVariable(var_data);
        response.setVariableResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a TriggerMessage message from the central */
bool DefaultChargePointEventsHandler::onTriggerMessage(const ocpp::messages::ocpp20::TriggerMessageReq& request,
                                                       ocpp::messages::ocpp20::TriggerMessageConf&      response,
                                                       std::string&                                     error,
                                                       std::string&                                     message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "TriggerMessage" << endl;

    response.status = TriggerMessageStatusEnumType::Rejected;

    return ret;
}

/** @brief Called on reception of a UnlockConnector message from the central */
bool DefaultChargePointEventsHandler::onUnlockConnector(const ocpp::messages::ocpp20::UnlockConnectorReq& request,
                                                        ocpp::messages::ocpp20::UnlockConnectorConf&      response,
                                                        std::string&                                      error,
                                                        std::string&                                      message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "UnlockConnector" << endl;

    response.status = UnlockStatusEnumType::UnknownConnector;

    return ret;
}

/** @brief Called on reception of a UnpublishFirmware message from the central */
bool DefaultChargePointEventsHandler::onUnpublishFirmware(const ocpp::messages::ocpp20::UnpublishFirmwareReq& request,
                                                          ocpp::messages::ocpp20::UnpublishFirmwareConf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "UnpublishFirmware" << endl;

    response.status = UnpublishFirmwareStatusEnumType::NoFirmware;

    return ret;
}

/** @brief Called on reception of a UpdateFirmware message from the central */
bool DefaultChargePointEventsHandler::onUpdateFirmware(const ocpp::messages::ocpp20::UpdateFirmwareReq& request,
                                                       ocpp::messages::ocpp20::UpdateFirmwareConf&      response,
                                                       std::string&                                     error,
                                                       std::string&                                     message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "UpdateFirmware" << endl;

    response.status = UpdateFirmwareStatusEnumType::Rejected;

    return ret;
}
