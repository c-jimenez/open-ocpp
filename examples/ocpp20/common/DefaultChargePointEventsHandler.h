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

#ifndef DEFAULTCHARGEPOINTEVENTSHANDLER_H
#define DEFAULTCHARGEPOINTEVENTSHANDLER_H

#include "IChargePoint20.h"
#include "IChargePointEventsHandler20.h"
#include "IDeviceModel20.h"

#include <filesystem>
#include <vector>

class ChargePointDemoConfig;

/** @brief Default charge point event handlers implementation for the examples */
class DefaultChargePointEventsHandler : public ocpp::chargepoint::ocpp20::IChargePointEventsHandler20,
                                        public ocpp::chargepoint::ocpp20::IDeviceModel20::IListener
{
  public:
    /** @brief Constructor */
    DefaultChargePointEventsHandler(ChargePointDemoConfig&                     config,
                                    ocpp::chargepoint::ocpp20::IDeviceModel20& device_model,
                                    const std::filesystem::path&               working_dir);

    /** @brief Destructor */
    virtual ~DefaultChargePointEventsHandler();

    /** @brief Set the associated Charge Point instance */
    void setChargePoint(ocpp::chargepoint::ocpp20::IChargePoint20& chargepoint) { m_chargepoint = &chargepoint; }

    /** @brief Indicate if the Charge Point is connected */
    bool isConnected() const { return m_is_connected; }

    // IDeviceModel20 interface

    /** @brief Called to retrieve the value of a variable */
    void getVariable(ocpp::types::ocpp20::GetVariableResultType& var) override;

    /** @brief Called to set the value of a variable */
    ocpp::types::ocpp20::SetVariableStatusEnumType setVariable(const ocpp::types::ocpp20::SetVariableDataType& var) override;

    // IChargePointEventsHandler20 interface

    /** @copydoc void IChargePointEventsHandler20::connectionStateChanged() */
    void connectionFailed() override;

    /** @copydoc void IChargePointEventsHandler20::connectionStateChanged(bool) */
    void connectionStateChanged(bool isConnected) override;

    // OCPP operations

    /** @brief Called on reception of a CancelReservation message from the central */
    bool onCancelReservation(const ocpp::messages::ocpp20::CancelReservationReq& request,
                             ocpp::messages::ocpp20::CancelReservationConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a CertificateSigned message from the central */
    bool onCertificateSigned(const ocpp::messages::ocpp20::CertificateSignedReq& request,
                             ocpp::messages::ocpp20::CertificateSignedConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a ChangeAvailability message from the central */
    bool onChangeAvailability(const ocpp::messages::ocpp20::ChangeAvailabilityReq& request,
                              ocpp::messages::ocpp20::ChangeAvailabilityConf&      response,
                              std::string&                                         error,
                              std::string&                                         message) override;

    /** @brief Called on reception of a ClearCache message from the central */
    bool onClearCache(const ocpp::messages::ocpp20::ClearCacheReq& request,
                      ocpp::messages::ocpp20::ClearCacheConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) override;

    /** @brief Called on reception of a ClearChargingProfile message from the central */
    bool onClearChargingProfile(const ocpp::messages::ocpp20::ClearChargingProfileReq& request,
                                ocpp::messages::ocpp20::ClearChargingProfileConf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a ClearDisplayMessage message from the central */
    bool onClearDisplayMessage(const ocpp::messages::ocpp20::ClearDisplayMessageReq& request,
                               ocpp::messages::ocpp20::ClearDisplayMessageConf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a ClearVariableMonitoring message from the central */
    bool onClearVariableMonitoring(const ocpp::messages::ocpp20::ClearVariableMonitoringReq& request,
                                   ocpp::messages::ocpp20::ClearVariableMonitoringConf&      response,
                                   std::string&                                              error,
                                   std::string&                                              message) override;

    /** @brief Called on reception of a CustomerInformation message from the central */
    bool onCustomerInformation(const ocpp::messages::ocpp20::CustomerInformationReq& request,
                               ocpp::messages::ocpp20::CustomerInformationConf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a DataTransfer message from the central */
    bool onDataTransfer(const ocpp::messages::ocpp20::DataTransferReq& request,
                        ocpp::messages::ocpp20::DataTransferConf&      response,
                        std::string&                                   error,
                        std::string&                                   message) override;

    /** @brief Called on reception of a DeleteCertificate message from the central */
    bool onDeleteCertificate(const ocpp::messages::ocpp20::DeleteCertificateReq& request,
                             ocpp::messages::ocpp20::DeleteCertificateConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a Get15118EVCertificate message from the central */
    bool onGet15118EVCertificate(const ocpp::messages::ocpp20::Get15118EVCertificateReq& request,
                                 ocpp::messages::ocpp20::Get15118EVCertificateConf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a GetBaseReport message from the central */
    bool onGetBaseReport(const ocpp::messages::ocpp20::GetBaseReportReq& request,
                         ocpp::messages::ocpp20::GetBaseReportConf&      response,
                         std::string&                                    error,
                         std::string&                                    message) override;

    /** @brief Called on reception of a GetCertificateStatus message from the central */
    bool onGetCertificateStatus(const ocpp::messages::ocpp20::GetCertificateStatusReq& request,
                                ocpp::messages::ocpp20::GetCertificateStatusConf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a GetChargingProfiles message from the central */
    bool onGetChargingProfiles(const ocpp::messages::ocpp20::GetChargingProfilesReq& request,
                               ocpp::messages::ocpp20::GetChargingProfilesConf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a GetCompositeSchedule message from the central */
    bool onGetCompositeSchedule(const ocpp::messages::ocpp20::GetCompositeScheduleReq& request,
                                ocpp::messages::ocpp20::GetCompositeScheduleConf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a GetDisplayMessages message from the central */
    bool onGetDisplayMessages(const ocpp::messages::ocpp20::GetDisplayMessagesReq& request,
                              ocpp::messages::ocpp20::GetDisplayMessagesConf&      response,
                              std::string&                                         error,
                              std::string&                                         message) override;

    /** @brief Called on reception of a GetInstalledCertificateIds message from the central */
    bool onGetInstalledCertificateIds(const ocpp::messages::ocpp20::GetInstalledCertificateIdsReq& request,
                                      ocpp::messages::ocpp20::GetInstalledCertificateIdsConf&      response,
                                      std::string&                                                 error,
                                      std::string&                                                 message) override;

    /** @brief Called on reception of a GetLocalListVersion message from the central */
    bool onGetLocalListVersion(const ocpp::messages::ocpp20::GetLocalListVersionReq& request,
                               ocpp::messages::ocpp20::GetLocalListVersionConf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a GetLog message from the central */
    bool onGetLog(const ocpp::messages::ocpp20::GetLogReq& request,
                  ocpp::messages::ocpp20::GetLogConf&      response,
                  std::string&                             error,
                  std::string&                             message) override;

    /** @brief Called on reception of a GetMonitoringReport message from the central */
    bool onGetMonitoringReport(const ocpp::messages::ocpp20::GetMonitoringReportReq& request,
                               ocpp::messages::ocpp20::GetMonitoringReportConf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a GetReport message from the central */
    bool onGetReport(const ocpp::messages::ocpp20::GetReportReq& request,
                     ocpp::messages::ocpp20::GetReportConf&      response,
                     std::string&                                error,
                     std::string&                                message) override;

    /** @brief Called on reception of a GetTransactionStatus message from the central */
    bool onGetTransactionStatus(const ocpp::messages::ocpp20::GetTransactionStatusReq& request,
                                ocpp::messages::ocpp20::GetTransactionStatusConf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a GetVariables message from the central */
    bool onGetVariables(const ocpp::messages::ocpp20::GetVariablesReq& request,
                        ocpp::messages::ocpp20::GetVariablesConf&      response,
                        std::string&                                   error,
                        std::string&                                   message) override;

    /** @brief Called on reception of a InstallCertificate message from the central */
    bool onInstallCertificate(const ocpp::messages::ocpp20::InstallCertificateReq& request,
                              ocpp::messages::ocpp20::InstallCertificateConf&      response,
                              std::string&                                         error,
                              std::string&                                         message) override;

    /** @brief Called on reception of a PublishFirmware message from the central */
    bool onPublishFirmware(const ocpp::messages::ocpp20::PublishFirmwareReq& request,
                           ocpp::messages::ocpp20::PublishFirmwareConf&      response,
                           std::string&                                      error,
                           std::string&                                      message) override;

    /** @brief Called on reception of a RequestStartTransaction message from the central */
    bool onRequestStartTransaction(const ocpp::messages::ocpp20::RequestStartTransactionReq& request,
                                   ocpp::messages::ocpp20::RequestStartTransactionConf&      response,
                                   std::string&                                              error,
                                   std::string&                                              message) override;

    /** @brief Called on reception of a RequestStopTransaction message from the central */
    bool onRequestStopTransaction(const ocpp::messages::ocpp20::RequestStopTransactionReq& request,
                                  ocpp::messages::ocpp20::RequestStopTransactionConf&      response,
                                  std::string&                                             error,
                                  std::string&                                             message) override;

    /** @brief Called on reception of a ReserveNow message from the central */
    bool onReserveNow(const ocpp::messages::ocpp20::ReserveNowReq& request,
                      ocpp::messages::ocpp20::ReserveNowConf&      response,
                      std::string&                                 error,
                      std::string&                                 message) override;

    /** @brief Called on reception of a Reset message from the central */
    bool onReset(const ocpp::messages::ocpp20::ResetReq& request,
                 ocpp::messages::ocpp20::ResetConf&      response,
                 std::string&                            error,
                 std::string&                            message) override;

    /** @brief Called on reception of a SendLocalList message from the central */
    bool onSendLocalList(const ocpp::messages::ocpp20::SendLocalListReq& request,
                         ocpp::messages::ocpp20::SendLocalListConf&      response,
                         std::string&                                    error,
                         std::string&                                    message) override;

    /** @brief Called on reception of a SetChargingProfile message from the central */
    bool onSetChargingProfile(const ocpp::messages::ocpp20::SetChargingProfileReq& request,
                              ocpp::messages::ocpp20::SetChargingProfileConf&      response,
                              std::string&                                         error,
                              std::string&                                         message) override;

    /** @brief Called on reception of a SetDisplayMessage message from the central */
    bool onSetDisplayMessage(const ocpp::messages::ocpp20::SetDisplayMessageReq& request,
                             ocpp::messages::ocpp20::SetDisplayMessageConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a SetMonitoringBase message from the central */
    bool onSetMonitoringBase(const ocpp::messages::ocpp20::SetMonitoringBaseReq& request,
                             ocpp::messages::ocpp20::SetMonitoringBaseConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a SetMonitoringLevel message from the central */
    bool onSetMonitoringLevel(const ocpp::messages::ocpp20::SetMonitoringLevelReq& request,
                              ocpp::messages::ocpp20::SetMonitoringLevelConf&      response,
                              std::string&                                         error,
                              std::string&                                         message) override;

    /** @brief Called on reception of a SetNetworkProfile message from the central */
    bool onSetNetworkProfile(const ocpp::messages::ocpp20::SetNetworkProfileReq& request,
                             ocpp::messages::ocpp20::SetNetworkProfileConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a SetVariableMonitoring message from the central */
    bool onSetVariableMonitoring(const ocpp::messages::ocpp20::SetVariableMonitoringReq& request,
                                 ocpp::messages::ocpp20::SetVariableMonitoringConf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a SetVariables message from the central */
    bool onSetVariables(const ocpp::messages::ocpp20::SetVariablesReq& request,
                        ocpp::messages::ocpp20::SetVariablesConf&      response,
                        std::string&                                   error,
                        std::string&                                   message) override;

    /** @brief Called on reception of a TriggerMessage message from the central */
    bool onTriggerMessage(const ocpp::messages::ocpp20::TriggerMessageReq& request,
                          ocpp::messages::ocpp20::TriggerMessageConf&      response,
                          std::string&                                     error,
                          std::string&                                     message) override;

    /** @brief Called on reception of a UnlockConnector message from the central */
    bool onUnlockConnector(const ocpp::messages::ocpp20::UnlockConnectorReq& request,
                           ocpp::messages::ocpp20::UnlockConnectorConf&      response,
                           std::string&                                      error,
                           std::string&                                      message) override;

    /** @brief Called on reception of a UnpublishFirmware message from the central */
    bool onUnpublishFirmware(const ocpp::messages::ocpp20::UnpublishFirmwareReq& request,
                             ocpp::messages::ocpp20::UnpublishFirmwareConf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a UpdateFirmware message from the central */
    bool onUpdateFirmware(const ocpp::messages::ocpp20::UpdateFirmwareReq& request,
                          ocpp::messages::ocpp20::UpdateFirmwareConf&      response,
                          std::string&                                     error,
                          std::string&                                     message) override;

  protected:
    /** @brief Get the configuration */
    ChargePointDemoConfig& config() { return m_config; }

  private:
    /** @brief Configuration */
    ChargePointDemoConfig& m_config;
    /** @brief Device model */
    ocpp::chargepoint::ocpp20::IDeviceModel20& m_device_model;
    /** @brief Associated Charge Point instance */
    ocpp::chargepoint::ocpp20::IChargePoint20* m_chargepoint;
    /** @brief Working directory */
    std::filesystem::path m_working_dir;
    /** @brief Indicate if the Charge Point is connected */
    bool m_is_connected;
};

#endif // DEFAULTCHARGEPOINTEVENTSHANDLER_H
