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

#include <filesystem>
#include <vector>

class ChargePointDemoConfig;

/** @brief Default charge point event handlers implementation for the examples */
class DefaultChargePointEventsHandler : public ocpp::chargepoint::ocpp20::IChargePointEventsHandler20
{
  public:
    /** @brief Constructor */
    DefaultChargePointEventsHandler(ChargePointDemoConfig& config, const std::filesystem::path& working_dir);

    /** @brief Destructor */
    virtual ~DefaultChargePointEventsHandler();

    /** @brief Set the associated Charge Point instance */
    void setChargePoint(ocpp::chargepoint::ocpp20::IChargePoint20& chargepoint) { m_chargepoint = &chargepoint; }

    /** @brief Indicate if the Charge Point is connected */
    bool isConnected() const { return m_is_connected; }

    // IChargePointEventsHandler20 interface

    /** @copydoc void IChargePointEventsHandler20::connectionStateChanged() */
    void connectionFailed() override;

    /** @copydoc void IChargePointEventsHandler20::connectionStateChanged(bool) */
    void connectionStateChanged(bool isConnected) override;

    // OCPP operations

    /** @brief Called on reception of a CancelReservation20 message from the central */
    bool onCancelReservation20(const ocpp::messages::ocpp20::CancelReservation20Req& request,
                               ocpp::messages::ocpp20::CancelReservation20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a CertificateSigned20 message from the central */
    bool onCertificateSigned20(const ocpp::messages::ocpp20::CertificateSigned20Req& request,
                               ocpp::messages::ocpp20::CertificateSigned20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a ChangeAvailability20 message from the central */
    bool onChangeAvailability20(const ocpp::messages::ocpp20::ChangeAvailability20Req& request,
                                ocpp::messages::ocpp20::ChangeAvailability20Conf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a ClearCache20 message from the central */
    bool onClearCache20(const ocpp::messages::ocpp20::ClearCache20Req& request,
                        ocpp::messages::ocpp20::ClearCache20Conf&      response,
                        std::string&                                   error,
                        std::string&                                   message) override;

    /** @brief Called on reception of a ClearChargingProfile20 message from the central */
    bool onClearChargingProfile20(const ocpp::messages::ocpp20::ClearChargingProfile20Req& request,
                                  ocpp::messages::ocpp20::ClearChargingProfile20Conf&      response,
                                  std::string&                                             error,
                                  std::string&                                             message) override;

    /** @brief Called on reception of a ClearDisplayMessage20 message from the central */
    bool onClearDisplayMessage20(const ocpp::messages::ocpp20::ClearDisplayMessage20Req& request,
                                 ocpp::messages::ocpp20::ClearDisplayMessage20Conf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a ClearVariableMonitoring20 message from the central */
    bool onClearVariableMonitoring20(const ocpp::messages::ocpp20::ClearVariableMonitoring20Req& request,
                                     ocpp::messages::ocpp20::ClearVariableMonitoring20Conf&      response,
                                     std::string&                                                error,
                                     std::string&                                                message) override;

    /** @brief Called on reception of a CustomerInformation20 message from the central */
    bool onCustomerInformation20(const ocpp::messages::ocpp20::CustomerInformation20Req& request,
                                 ocpp::messages::ocpp20::CustomerInformation20Conf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a DataTransfer20 message from the central */
    bool onDataTransfer20(const ocpp::messages::ocpp20::DataTransfer20Req& request,
                          ocpp::messages::ocpp20::DataTransfer20Conf&      response,
                          std::string&                                     error,
                          std::string&                                     message) override;

    /** @brief Called on reception of a DeleteCertificate20 message from the central */
    bool onDeleteCertificate20(const ocpp::messages::ocpp20::DeleteCertificate20Req& request,
                               ocpp::messages::ocpp20::DeleteCertificate20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a Get15118EVCertificate20 message from the central */
    bool onGet15118EVCertificate20(const ocpp::messages::ocpp20::Get15118EVCertificate20Req& request,
                                   ocpp::messages::ocpp20::Get15118EVCertificate20Conf&      response,
                                   std::string&                                              error,
                                   std::string&                                              message) override;

    /** @brief Called on reception of a GetBaseReport20 message from the central */
    bool onGetBaseReport20(const ocpp::messages::ocpp20::GetBaseReport20Req& request,
                           ocpp::messages::ocpp20::GetBaseReport20Conf&      response,
                           std::string&                                      error,
                           std::string&                                      message) override;

    /** @brief Called on reception of a GetCertificateStatus20 message from the central */
    bool onGetCertificateStatus20(const ocpp::messages::ocpp20::GetCertificateStatus20Req& request,
                                  ocpp::messages::ocpp20::GetCertificateStatus20Conf&      response,
                                  std::string&                                             error,
                                  std::string&                                             message) override;

    /** @brief Called on reception of a GetChargingProfiles20 message from the central */
    bool onGetChargingProfiles20(const ocpp::messages::ocpp20::GetChargingProfiles20Req& request,
                                 ocpp::messages::ocpp20::GetChargingProfiles20Conf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a GetCompositeSchedule20 message from the central */
    bool onGetCompositeSchedule20(const ocpp::messages::ocpp20::GetCompositeSchedule20Req& request,
                                  ocpp::messages::ocpp20::GetCompositeSchedule20Conf&      response,
                                  std::string&                                             error,
                                  std::string&                                             message) override;

    /** @brief Called on reception of a GetDisplayMessages20 message from the central */
    bool onGetDisplayMessages20(const ocpp::messages::ocpp20::GetDisplayMessages20Req& request,
                                ocpp::messages::ocpp20::GetDisplayMessages20Conf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a GetInstalledCertificateIds20 message from the central */
    bool onGetInstalledCertificateIds20(const ocpp::messages::ocpp20::GetInstalledCertificateIds20Req& request,
                                        ocpp::messages::ocpp20::GetInstalledCertificateIds20Conf&      response,
                                        std::string&                                                   error,
                                        std::string&                                                   message) override;

    /** @brief Called on reception of a GetLocalListVersion20 message from the central */
    bool onGetLocalListVersion20(const ocpp::messages::ocpp20::GetLocalListVersion20Req& request,
                                 ocpp::messages::ocpp20::GetLocalListVersion20Conf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a GetLog20 message from the central */
    bool onGetLog20(const ocpp::messages::ocpp20::GetLog20Req& request,
                    ocpp::messages::ocpp20::GetLog20Conf&      response,
                    std::string&                               error,
                    std::string&                               message) override;

    /** @brief Called on reception of a GetMonitoringReport20 message from the central */
    bool onGetMonitoringReport20(const ocpp::messages::ocpp20::GetMonitoringReport20Req& request,
                                 ocpp::messages::ocpp20::GetMonitoringReport20Conf&      response,
                                 std::string&                                            error,
                                 std::string&                                            message) override;

    /** @brief Called on reception of a GetReport20 message from the central */
    bool onGetReport20(const ocpp::messages::ocpp20::GetReport20Req& request,
                       ocpp::messages::ocpp20::GetReport20Conf&      response,
                       std::string&                                  error,
                       std::string&                                  message) override;

    /** @brief Called on reception of a GetTransactionStatus20 message from the central */
    bool onGetTransactionStatus20(const ocpp::messages::ocpp20::GetTransactionStatus20Req& request,
                                  ocpp::messages::ocpp20::GetTransactionStatus20Conf&      response,
                                  std::string&                                             error,
                                  std::string&                                             message) override;

    /** @brief Called on reception of a GetVariables20 message from the central */
    bool onGetVariables20(const ocpp::messages::ocpp20::GetVariables20Req& request,
                          ocpp::messages::ocpp20::GetVariables20Conf&      response,
                          std::string&                                     error,
                          std::string&                                     message) override;

    /** @brief Called on reception of a InstallCertificate20 message from the central */
    bool onInstallCertificate20(const ocpp::messages::ocpp20::InstallCertificate20Req& request,
                                ocpp::messages::ocpp20::InstallCertificate20Conf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a PublishFirmware20 message from the central */
    bool onPublishFirmware20(const ocpp::messages::ocpp20::PublishFirmware20Req& request,
                             ocpp::messages::ocpp20::PublishFirmware20Conf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a RequestStartTransaction20 message from the central */
    bool onRequestStartTransaction20(const ocpp::messages::ocpp20::RequestStartTransaction20Req& request,
                                     ocpp::messages::ocpp20::RequestStartTransaction20Conf&      response,
                                     std::string&                                                error,
                                     std::string&                                                message) override;

    /** @brief Called on reception of a RequestStopTransaction20 message from the central */
    bool onRequestStopTransaction20(const ocpp::messages::ocpp20::RequestStopTransaction20Req& request,
                                    ocpp::messages::ocpp20::RequestStopTransaction20Conf&      response,
                                    std::string&                                               error,
                                    std::string&                                               message) override;

    /** @brief Called on reception of a ReserveNow20 message from the central */
    bool onReserveNow20(const ocpp::messages::ocpp20::ReserveNow20Req& request,
                        ocpp::messages::ocpp20::ReserveNow20Conf&      response,
                        std::string&                                   error,
                        std::string&                                   message) override;

    /** @brief Called on reception of a Reset20 message from the central */
    bool onReset20(const ocpp::messages::ocpp20::Reset20Req& request,
                   ocpp::messages::ocpp20::Reset20Conf&      response,
                   std::string&                              error,
                   std::string&                              message) override;

    /** @brief Called on reception of a SendLocalList20 message from the central */
    bool onSendLocalList20(const ocpp::messages::ocpp20::SendLocalList20Req& request,
                           ocpp::messages::ocpp20::SendLocalList20Conf&      response,
                           std::string&                                      error,
                           std::string&                                      message) override;

    /** @brief Called on reception of a SetChargingProfile20 message from the central */
    bool onSetChargingProfile20(const ocpp::messages::ocpp20::SetChargingProfile20Req& request,
                                ocpp::messages::ocpp20::SetChargingProfile20Conf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a SetDisplayMessage20 message from the central */
    bool onSetDisplayMessage20(const ocpp::messages::ocpp20::SetDisplayMessage20Req& request,
                               ocpp::messages::ocpp20::SetDisplayMessage20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a SetMonitoringBase20 message from the central */
    bool onSetMonitoringBase20(const ocpp::messages::ocpp20::SetMonitoringBase20Req& request,
                               ocpp::messages::ocpp20::SetMonitoringBase20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a SetMonitoringLevel20 message from the central */
    bool onSetMonitoringLevel20(const ocpp::messages::ocpp20::SetMonitoringLevel20Req& request,
                                ocpp::messages::ocpp20::SetMonitoringLevel20Conf&      response,
                                std::string&                                           error,
                                std::string&                                           message) override;

    /** @brief Called on reception of a SetNetworkProfile20 message from the central */
    bool onSetNetworkProfile20(const ocpp::messages::ocpp20::SetNetworkProfile20Req& request,
                               ocpp::messages::ocpp20::SetNetworkProfile20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a SetVariableMonitoring20 message from the central */
    bool onSetVariableMonitoring20(const ocpp::messages::ocpp20::SetVariableMonitoring20Req& request,
                                   ocpp::messages::ocpp20::SetVariableMonitoring20Conf&      response,
                                   std::string&                                              error,
                                   std::string&                                              message) override;

    /** @brief Called on reception of a SetVariables20 message from the central */
    bool onSetVariables20(const ocpp::messages::ocpp20::SetVariables20Req& request,
                          ocpp::messages::ocpp20::SetVariables20Conf&      response,
                          std::string&                                     error,
                          std::string&                                     message) override;

    /** @brief Called on reception of a TriggerMessage20 message from the central */
    bool onTriggerMessage20(const ocpp::messages::ocpp20::TriggerMessage20Req& request,
                            ocpp::messages::ocpp20::TriggerMessage20Conf&      response,
                            std::string&                                       error,
                            std::string&                                       message) override;

    /** @brief Called on reception of a UnlockConnector20 message from the central */
    bool onUnlockConnector20(const ocpp::messages::ocpp20::UnlockConnector20Req& request,
                             ocpp::messages::ocpp20::UnlockConnector20Conf&      response,
                             std::string&                                        error,
                             std::string&                                        message) override;

    /** @brief Called on reception of a UnpublishFirmware20 message from the central */
    bool onUnpublishFirmware20(const ocpp::messages::ocpp20::UnpublishFirmware20Req& request,
                               ocpp::messages::ocpp20::UnpublishFirmware20Conf&      response,
                               std::string&                                          error,
                               std::string&                                          message) override;

    /** @brief Called on reception of a UpdateFirmware20 message from the central */
    bool onUpdateFirmware20(const ocpp::messages::ocpp20::UpdateFirmware20Req& request,
                            ocpp::messages::ocpp20::UpdateFirmware20Conf&      response,
                            std::string&                                       error,
                            std::string&                                       message) override;

  protected:
    /** @brief Get the configuration */
    ChargePointDemoConfig& config() { return m_config; }

  private:
    /** @brief Configuration */
    ChargePointDemoConfig& m_config;
    /** @brief Associated Charge Point instance */
    ocpp::chargepoint::ocpp20::IChargePoint20* m_chargepoint;
    /** @brief Working directory */
    std::filesystem::path m_working_dir;
    /** @brief Indicate if the Charge Point is connected */
    bool m_is_connected;
};

#endif // DEFAULTCHARGEPOINTEVENTSHANDLER_H
