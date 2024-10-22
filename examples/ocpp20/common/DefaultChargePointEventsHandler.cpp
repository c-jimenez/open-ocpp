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

#include <fstream>
#include <iostream>

// With MSVC compiler, the system() call returns directly the command's return value
#ifdef _MSC_VER
#define WEXITSTATUS(x) (x)
#endif // _MSC_VER

using namespace std;
using namespace ocpp::types;
using namespace ocpp::types::ocpp20;

/** @brief Constructor */
DefaultChargePointEventsHandler::DefaultChargePointEventsHandler(ChargePointDemoConfig& config, const std::filesystem::path& working_dir)
    : m_config(config), m_chargepoint(nullptr), m_working_dir(working_dir), m_is_connected(false)
{
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

// OCPP operations

/** @brief Called on reception of a CancelReservation20 message from the central */
bool DefaultChargePointEventsHandler::onCancelReservation20(const ocpp::messages::ocpp20::CancelReservation20Req& request,
                                                            ocpp::messages::ocpp20::CancelReservation20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "CancelReservation" << endl;

    response.status = CancelReservationStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a CertificateSigned20 message from the central */
bool DefaultChargePointEventsHandler::onCertificateSigned20(const ocpp::messages::ocpp20::CertificateSigned20Req& request,
                                                            ocpp::messages::ocpp20::CertificateSigned20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "CertificateSigned" << endl;

    response.status = CertificateSignedStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a ChangeAvailability20 message from the central */
bool DefaultChargePointEventsHandler::onChangeAvailability20(const ocpp::messages::ocpp20::ChangeAvailability20Req& request,
                                                             ocpp::messages::ocpp20::ChangeAvailability20Conf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ChangeAvailability" << endl;

    response.status = ChangeAvailabilityStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a ClearCache20 message from the central */
bool DefaultChargePointEventsHandler::onClearCache20(const ocpp::messages::ocpp20::ClearCache20Req& request,
                                                     ocpp::messages::ocpp20::ClearCache20Conf&      response,
                                                     std::string&                                   error,
                                                     std::string&                                   message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ClearCache" << endl;

    response.status = ClearCacheStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a ClearChargingProfile20 message from the central */
bool DefaultChargePointEventsHandler::onClearChargingProfile20(const ocpp::messages::ocpp20::ClearChargingProfile20Req& request,
                                                               ocpp::messages::ocpp20::ClearChargingProfile20Conf&      response,
                                                               std::string&                                             error,
                                                               std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ClearChargingProfile" << endl;

    response.status = ClearChargingProfileStatusEnumType20::Unknown;

    return ret;
}

/** @brief Called on reception of a ClearDisplayMessage20 message from the central */
bool DefaultChargePointEventsHandler::onClearDisplayMessage20(const ocpp::messages::ocpp20::ClearDisplayMessage20Req& request,
                                                              ocpp::messages::ocpp20::ClearDisplayMessage20Conf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ClearDisplayMessage" << endl;

    response.status = ClearMessageStatusEnumType20::Unknown;

    return ret;
}

/** @brief Called on reception of a ClearVariableMonitoring20 message from the central */
bool DefaultChargePointEventsHandler::onClearVariableMonitoring20(const ocpp::messages::ocpp20::ClearVariableMonitoring20Req& request,
                                                                  ocpp::messages::ocpp20::ClearVariableMonitoring20Conf&      response,
                                                                  std::string&                                                error,
                                                                  std::string&                                                message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "ClearVariableMonitoring" << endl;

    for (const auto& id : request.id)
    {
        (void)id;

        ClearMonitoringResultType20 result;
        result.status = ClearMonitoringStatusEnumType20::Rejected;
        response.clearMonitoringResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a CustomerInformation20 message from the central */
bool DefaultChargePointEventsHandler::onCustomerInformation20(const ocpp::messages::ocpp20::CustomerInformation20Req& request,
                                                              ocpp::messages::ocpp20::CustomerInformation20Conf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "CustomerInformation" << endl;

    response.status = CustomerInformationStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a DataTransfer20 message from the central */
bool DefaultChargePointEventsHandler::onDataTransfer20(const ocpp::messages::ocpp20::DataTransfer20Req& request,
                                                       ocpp::messages::ocpp20::DataTransfer20Conf&      response,
                                                       std::string&                                     error,
                                                       std::string&                                     message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "DataTransfer" << endl;

    response.status = DataTransferStatusEnumType20::UnknownVendorId;

    return ret;
}

/** @brief Called on reception of a DeleteCertificate20 message from the central */
bool DefaultChargePointEventsHandler::onDeleteCertificate20(const ocpp::messages::ocpp20::DeleteCertificate20Req& request,
                                                            ocpp::messages::ocpp20::DeleteCertificate20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "DeleteCertificate" << endl;

    response.status = DeleteCertificateStatusEnumType20::NotFound;

    return ret;
}

/** @brief Called on reception of a Get15118EVCertificate20 message from the central */
bool DefaultChargePointEventsHandler::onGet15118EVCertificate20(const ocpp::messages::ocpp20::Get15118EVCertificate20Req& request,
                                                                ocpp::messages::ocpp20::Get15118EVCertificate20Conf&      response,
                                                                std::string&                                              error,
                                                                std::string&                                              message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "Get15118EVCertificate" << endl;

    response.status = Iso15118EVCertificateStatusEnumType20::Failed;

    return ret;
}

/** @brief Called on reception of a GetBaseReport20 message from the central */
bool DefaultChargePointEventsHandler::onGetBaseReport20(const ocpp::messages::ocpp20::GetBaseReport20Req& request,
                                                        ocpp::messages::ocpp20::GetBaseReport20Conf&      response,
                                                        std::string&                                      error,
                                                        std::string&                                      message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetBaseReport" << endl;

    response.status = GenericDeviceModelStatusEnumType20::NotSupported;

    return ret;
}

/** @brief Called on reception of a GetCertificateStatus20 message from the central */
bool DefaultChargePointEventsHandler::onGetCertificateStatus20(const ocpp::messages::ocpp20::GetCertificateStatus20Req& request,
                                                               ocpp::messages::ocpp20::GetCertificateStatus20Conf&      response,
                                                               std::string&                                             error,
                                                               std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetCertificateStatus" << endl;

    response.status = GetCertificateStatusEnumType20::Failed;

    return ret;
}

/** @brief Called on reception of a GetChargingProfiles20 message from the central */
bool DefaultChargePointEventsHandler::onGetChargingProfiles20(const ocpp::messages::ocpp20::GetChargingProfiles20Req& request,
                                                              ocpp::messages::ocpp20::GetChargingProfiles20Conf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetChargingProfiles" << endl;

    response.status = GetChargingProfileStatusEnumType20::NoProfiles;

    return ret;
}

/** @brief Called on reception of a GetCompositeSchedule20 message from the central */
bool DefaultChargePointEventsHandler::onGetCompositeSchedule20(const ocpp::messages::ocpp20::GetCompositeSchedule20Req& request,
                                                               ocpp::messages::ocpp20::GetCompositeSchedule20Conf&      response,
                                                               std::string&                                             error,
                                                               std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetCompositeSchedule" << endl;

    response.status = GenericStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a GetDisplayMessages20 message from the central */
bool DefaultChargePointEventsHandler::onGetDisplayMessages20(const ocpp::messages::ocpp20::GetDisplayMessages20Req& request,
                                                             ocpp::messages::ocpp20::GetDisplayMessages20Conf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetDisplayMessages" << endl;

    response.status = GetDisplayMessagesStatusEnumType20::Unknown;

    return ret;
}

/** @brief Called on reception of a GetInstalledCertificateIds20 message from the central */
bool DefaultChargePointEventsHandler::onGetInstalledCertificateIds20(const ocpp::messages::ocpp20::GetInstalledCertificateIds20Req& request,
                                                                     ocpp::messages::ocpp20::GetInstalledCertificateIds20Conf& response,
                                                                     std::string&                                              error,
                                                                     std::string&                                              message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetInstalledCertificateIds" << endl;

    response.status = GetInstalledCertificateStatusEnumType20::NotFound;

    return ret;
}

/** @brief Called on reception of a GetLocalListVersion20 message from the central */
bool DefaultChargePointEventsHandler::onGetLocalListVersion20(const ocpp::messages::ocpp20::GetLocalListVersion20Req& request,
                                                              ocpp::messages::ocpp20::GetLocalListVersion20Conf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetLocalListVersion" << endl;

    response.versionNumber = 0;

    return ret;
}

/** @brief Called on reception of a GetLog20 message from the central */
bool DefaultChargePointEventsHandler::onGetLog20(const ocpp::messages::ocpp20::GetLog20Req& request,
                                                 ocpp::messages::ocpp20::GetLog20Conf&      response,
                                                 std::string&                               error,
                                                 std::string&                               message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetLog" << endl;

    response.status = LogStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a GetMonitoringReport20 message from the central */
bool DefaultChargePointEventsHandler::onGetMonitoringReport20(const ocpp::messages::ocpp20::GetMonitoringReport20Req& request,
                                                              ocpp::messages::ocpp20::GetMonitoringReport20Conf&      response,
                                                              std::string&                                            error,
                                                              std::string&                                            message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetMonitoringReport" << endl;

    response.status = GenericDeviceModelStatusEnumType20::NotSupported;

    return ret;
}

/** @brief Called on reception of a GetReport20 message from the central */
bool DefaultChargePointEventsHandler::onGetReport20(const ocpp::messages::ocpp20::GetReport20Req& request,
                                                    ocpp::messages::ocpp20::GetReport20Conf&      response,
                                                    std::string&                                  error,
                                                    std::string&                                  message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetReport" << endl;

    response.status = GenericDeviceModelStatusEnumType20::NotSupported;

    return ret;
}

/** @brief Called on reception of a GetTransactionStatus20 message from the central */
bool DefaultChargePointEventsHandler::onGetTransactionStatus20(const ocpp::messages::ocpp20::GetTransactionStatus20Req& request,
                                                               ocpp::messages::ocpp20::GetTransactionStatus20Conf&      response,
                                                               std::string&                                             error,
                                                               std::string&                                             message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "GetTransactionStatus" << endl;

    response.messagesInQueue = false;

    return ret;
}

/** @brief Called on reception of a GetVariables20 message from the central */
bool DefaultChargePointEventsHandler::onGetVariables20(const ocpp::messages::ocpp20::GetVariables20Req& request,
                                                       ocpp::messages::ocpp20::GetVariables20Conf&      response,
                                                       std::string&                                     error,
                                                       std::string&                                     message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "GetVariables" << endl;

    for (const auto& var_data : request.getVariableData)
    {
        (void)var_data;

        GetVariableResultType20 result;
        result.attributeStatus = GetVariableStatusEnumType20::UnknownVariable;
        response.getVariableResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a InstallCertificate20 message from the central */
bool DefaultChargePointEventsHandler::onInstallCertificate20(const ocpp::messages::ocpp20::InstallCertificate20Req& request,
                                                             ocpp::messages::ocpp20::InstallCertificate20Conf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "InstallCertificate" << endl;

    response.status = InstallCertificateStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a PublishFirmware20 message from the central */
bool DefaultChargePointEventsHandler::onPublishFirmware20(const ocpp::messages::ocpp20::PublishFirmware20Req& request,
                                                          ocpp::messages::ocpp20::PublishFirmware20Conf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "PublishFirmware" << endl;

    response.status = GenericStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a RequestStartTransaction20 message from the central */
bool DefaultChargePointEventsHandler::onRequestStartTransaction20(const ocpp::messages::ocpp20::RequestStartTransaction20Req& request,
                                                                  ocpp::messages::ocpp20::RequestStartTransaction20Conf&      response,
                                                                  std::string&                                                error,
                                                                  std::string&                                                message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "RequestStartTransaction" << endl;

    response.status = RequestStartStopStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a RequestStopTransaction20 message from the central */
bool DefaultChargePointEventsHandler::onRequestStopTransaction20(const ocpp::messages::ocpp20::RequestStopTransaction20Req& request,
                                                                 ocpp::messages::ocpp20::RequestStopTransaction20Conf&      response,
                                                                 std::string&                                               error,
                                                                 std::string&                                               message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "RequestStopTransaction" << endl;

    response.status = RequestStartStopStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a ReserveNow20 message from the central */
bool DefaultChargePointEventsHandler::onReserveNow20(const ocpp::messages::ocpp20::ReserveNow20Req& request,
                                                     ocpp::messages::ocpp20::ReserveNow20Conf&      response,
                                                     std::string&                                   error,
                                                     std::string&                                   message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "ReserveNow" << endl;

    response.status = ReserveNowStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a Reset20 message from the central */
bool DefaultChargePointEventsHandler::onReset20(const ocpp::messages::ocpp20::Reset20Req& request,
                                                ocpp::messages::ocpp20::Reset20Conf&      response,
                                                std::string&                              error,
                                                std::string&                              message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "Reset" << endl;

    response.status = ResetStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a SendLocalList20 message from the central */
bool DefaultChargePointEventsHandler::onSendLocalList20(const ocpp::messages::ocpp20::SendLocalList20Req& request,
                                                        ocpp::messages::ocpp20::SendLocalList20Conf&      response,
                                                        std::string&                                      error,
                                                        std::string&                                      message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SendLocalList" << endl;

    response.status = SendLocalListStatusEnumType20::Failed;

    return ret;
}

/** @brief Called on reception of a SetChargingProfile20 message from the central */
bool DefaultChargePointEventsHandler::onSetChargingProfile20(const ocpp::messages::ocpp20::SetChargingProfile20Req& request,
                                                             ocpp::messages::ocpp20::SetChargingProfile20Conf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetChargingProfile" << endl;

    response.status = ChargingProfileStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a SetDisplayMessage20 message from the central */
bool DefaultChargePointEventsHandler::onSetDisplayMessage20(const ocpp::messages::ocpp20::SetDisplayMessage20Req& request,
                                                            ocpp::messages::ocpp20::SetDisplayMessage20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetDisplayMessage" << endl;

    response.status = DisplayMessageStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a SetMonitoringBase20 message from the central */
bool DefaultChargePointEventsHandler::onSetMonitoringBase20(const ocpp::messages::ocpp20::SetMonitoringBase20Req& request,
                                                            ocpp::messages::ocpp20::SetMonitoringBase20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetMonitoringBase" << endl;

    response.status = GenericDeviceModelStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a SetMonitoringLevel20 message from the central */
bool DefaultChargePointEventsHandler::onSetMonitoringLevel20(const ocpp::messages::ocpp20::SetMonitoringLevel20Req& request,
                                                             ocpp::messages::ocpp20::SetMonitoringLevel20Conf&      response,
                                                             std::string&                                           error,
                                                             std::string&                                           message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetMonitoringLevel" << endl;

    response.status = GenericStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a SetNetworkProfile20 message from the central */
bool DefaultChargePointEventsHandler::onSetNetworkProfile20(const ocpp::messages::ocpp20::SetNetworkProfile20Req& request,
                                                            ocpp::messages::ocpp20::SetNetworkProfile20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "SetNetworkProfile" << endl;

    response.status = SetNetworkProfileStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a SetVariableMonitoring20 message from the central */
bool DefaultChargePointEventsHandler::onSetVariableMonitoring20(const ocpp::messages::ocpp20::SetVariableMonitoring20Req& request,
                                                                ocpp::messages::ocpp20::SetVariableMonitoring20Conf&      response,
                                                                std::string&                                              error,
                                                                std::string&                                              message)
{
    bool ret = true;

    (void)error;
    (void)message;

    cout << "SetVariableMonitoring" << endl;

    for (const auto& data : request.setMonitoringData)
    {
        (void)data;

        SetMonitoringResultType20 result;
        result.status    = SetMonitoringStatusEnumType20::Rejected;
        result.type      = data.type;
        result.component = data.component;
        result.variable  = data.variable;
        result.severity  = data.severity;
        response.setMonitoringResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a SetVariables20 message from the central */
bool DefaultChargePointEventsHandler::onSetVariables20(const ocpp::messages::ocpp20::SetVariables20Req& request,
                                                       ocpp::messages::ocpp20::SetVariables20Conf&      response,
                                                       std::string&                                     error,
                                                       std::string&                                     message)
{
    bool ret = true;

    (void)response;
    (void)error;
    (void)message;

    cout << "SetVariables" << endl;

    for (const auto& var_data : request.setVariableData)
    {
        SetVariableResultType20 result;
        result.attributeStatus = SetVariableStatusEnumType20::Rejected;
        result.component       = var_data.component;
        result.variable        = var_data.variable;
        response.setVariableResult.push_back(result);
    }

    return ret;
}

/** @brief Called on reception of a TriggerMessage20 message from the central */
bool DefaultChargePointEventsHandler::onTriggerMessage20(const ocpp::messages::ocpp20::TriggerMessage20Req& request,
                                                         ocpp::messages::ocpp20::TriggerMessage20Conf&      response,
                                                         std::string&                                       error,
                                                         std::string&                                       message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "TriggerMessage" << endl;

    response.status = TriggerMessageStatusEnumType20::Rejected;

    return ret;
}

/** @brief Called on reception of a UnlockConnector20 message from the central */
bool DefaultChargePointEventsHandler::onUnlockConnector20(const ocpp::messages::ocpp20::UnlockConnector20Req& request,
                                                          ocpp::messages::ocpp20::UnlockConnector20Conf&      response,
                                                          std::string&                                        error,
                                                          std::string&                                        message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "UnlockConnector" << endl;

    response.status = UnlockStatusEnumType20::UnknownConnector;

    return ret;
}

/** @brief Called on reception of a UnpublishFirmware20 message from the central */
bool DefaultChargePointEventsHandler::onUnpublishFirmware20(const ocpp::messages::ocpp20::UnpublishFirmware20Req& request,
                                                            ocpp::messages::ocpp20::UnpublishFirmware20Conf&      response,
                                                            std::string&                                          error,
                                                            std::string&                                          message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "UnpublishFirmware" << endl;

    response.status = UnpublishFirmwareStatusEnumType20::NoFirmware;

    return ret;
}

/** @brief Called on reception of a UpdateFirmware20 message from the central */
bool DefaultChargePointEventsHandler::onUpdateFirmware20(const ocpp::messages::ocpp20::UpdateFirmware20Req& request,
                                                         ocpp::messages::ocpp20::UpdateFirmware20Conf&      response,
                                                         std::string&                                       error,
                                                         std::string&                                       message)
{
    bool ret = true;

    (void)request;
    (void)error;
    (void)message;

    cout << "UpdateFirmware" << endl;

    response.status = UpdateFirmwareStatusEnumType20::Rejected;

    return ret;
}
