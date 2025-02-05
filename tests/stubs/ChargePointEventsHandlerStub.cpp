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

#include "ChargePointEventsHandlerStub.h"

#include <sstream>

using namespace std;
using namespace ocpp::types;

/** @brief Constructor */
ChargePointEventsHandlerStub::ChargePointEventsHandlerStub()
    : m_calls(),
      m_call_results(),
      m_availability_status(AvailabilityStatus::Accepted),
      m_tx_start_stop_value(0),
      m_data_transfer_status(DataTransferStatus::Accepted),
      m_unlock_status(UnlockStatus::Unlocked),
      m_diag_file("")
{
}

/** @brief Destructor */
ChargePointEventsHandlerStub::~ChargePointEventsHandlerStub() { }

/** @copydoc void IChargePointEventsHandler::connectionStateChanged(ocpp::types::RegistrationStatus) */
void ChargePointEventsHandlerStub::connectionFailed(ocpp::types::RegistrationStatus status)
{
    m_calls["connectionFailed"] = {{"status", RegistrationStatusHelper.toString(status)}};
}

/** @copydoc void IChargePointEventsHandler::connectionStateChanged(bool) */
void ChargePointEventsHandlerStub::connectionStateChanged(bool isConnected)
{
    m_calls["connectionStateChanged"] = {{"isConnected", std::to_string(isConnected)}};
}

/** @copydoc void IChargePointEventsHandler::bootNotification(ocpp::types::RegistrationStatus, const ocpp::types::DateTime&) */
void ChargePointEventsHandlerStub::bootNotification(ocpp::types::RegistrationStatus status, const ocpp::types::DateTime& datetime)
{
    m_calls["bootNotification"] = {{"status", RegistrationStatusHelper.toString(status)}, {"dateTime", datetime.str()}};
}

/** @copydoc void IChargePointEventsHandler::datetimeReceived(const ocpp::types::DateTime&) */
void ChargePointEventsHandlerStub::datetimeReceived(const ocpp::types::DateTime& datetime)
{
    m_calls["datetimeReceived"] = {{"dateTime", datetime.str()}};
}

/** @copydoc AvailabilityStatus IChargePointEventsHandler::changeAvailabilityRequested(unsigned int, ocpp::types::AvailabilityType) */
ocpp::types::AvailabilityStatus ChargePointEventsHandlerStub::changeAvailabilityRequested(unsigned int                  connector_id,
                                                                                          ocpp::types::AvailabilityType availability)
{
    m_calls["changeAvailabilityRequested"] = {{"connector_id", std::to_string(connector_id)},
                                              {"availability", AvailabilityTypeHelper.toString(availability)}};

    return m_availability_status;
}

/** @copydoc int IChargePointEventsHandler::getTxStartStopMeterValue(unsigned int) */
int ChargePointEventsHandlerStub::getTxStartStopMeterValue(unsigned int connector_id)
{
    m_calls["getTxStartStopMeterValue"] = {{"connector_id", std::to_string(connector_id)}};
    return m_tx_start_stop_value;
}

/** @copydoc void IChargePointEventsHandler::reservationStarted(unsigned int) */
void ChargePointEventsHandlerStub::reservationStarted(unsigned int connector_id)
{
    m_calls["reservationStarted"] = {{"connector_id", std::to_string(connector_id)}};
}

/** @copydoc void IChargePointEventsHandler::reservationEnded(unsigned int, bool) */
void ChargePointEventsHandlerStub::reservationEnded(unsigned int connector_id, bool canceled)
{
    m_calls["reservationStarted"] = {{"connector_id", std::to_string(connector_id)}, {"canceled", std::to_string(canceled)}};
}

/** @copydoc ocpp::types::DataTransferStatus IChargePointEventsHandler::dataTransferRequested(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  std::string&) */
ocpp::types::DataTransferStatus ChargePointEventsHandlerStub::dataTransferRequested(const std::string& vendor_id,
                                                                                    const std::string& message_id,
                                                                                    const std::string& request_data,
                                                                                    std::string&       response_data)
{
    (void)response_data;
    m_calls["dataTransferRequested"] = {{"vendor_id", vendor_id}, {"message_id", message_id}, {"request_data", request_data}};
    return m_data_transfer_status;
}

/** @copydoc bool getMeterValue(unsigned int, const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>&, ocpp::types::MeterValue&) */
bool ChargePointEventsHandlerStub::getMeterValue(
    unsigned int                                                                        connector_id,
    const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
    ocpp::types::MeterValue&                                                            meter_value)
{
    m_calls["getMeterValue"] = {{"connector_id", std::to_string(connector_id)},
                                {"measurand", MeasurandHelper.toString(measurand.first)},
                                {"phase", (measurand.second.isSet() ? PhaseHelper.toString(measurand.second) : "not set")}};
    bool ret                 = callResult("getMeterValue");
    if (ret)
    {
        std::stringstream ss;
        ss << MeasurandHelper.toString(measurand.first) << "-";
        ss << (measurand.second.isSet() ? PhaseHelper.toString(measurand.second) : "none");
        ret = false;

        const auto iter_con = m_meter_values.find(connector_id);
        if (iter_con != m_meter_values.cend())
        {
            const auto iter_value = iter_con->second.find(ss.str());
            if (iter_value != iter_con->second.cend())
            {
                meter_value.sampledValue.insert(
                    meter_value.sampledValue.cend(), iter_value->second.sampledValue.begin(), iter_value->second.sampledValue.end());
                ret = true;
            }
        }
    }
    return ret;
}

/** @copydoc bool IChargePointEventsHandler::remoteStartTransactionRequested(unsigned int, const std::string&) */
bool ChargePointEventsHandlerStub::remoteStartTransactionRequested(unsigned int connector_id, const std::string& id_tag)
{
    m_calls["remoteStartTransactionRequested"] = {{"connector_id", std::to_string(connector_id)}, {"id_tag", id_tag}};
    return callResult("remoteStartTransactionRequested");
}

/** @copydoc bool IChargePointEventsHandler::remoteStopTransactionRequested(unsigned int) */
bool ChargePointEventsHandlerStub::remoteStopTransactionRequested(unsigned int connector_id)
{
    m_calls["remoteStopTransactionRequested"] = {{"connector_id", std::to_string(connector_id)}};
    return callResult("remoteStopTransactionRequested");
}

/** @copydoc void IChargePointEventsHandler::transactionDeAuthorized(unsigned int) */
void ChargePointEventsHandlerStub::transactionDeAuthorized(unsigned int connector_id)
{
    m_calls["transactionDeAuthorized"] = {{"connector_id", std::to_string(connector_id)}};
}

/** @copydoc bool IChargePointEventsHandler::getLocalLimitationsSchedule(unsigned int, unsigned int, ocpp::types::ChargingSchedule&) */
bool ChargePointEventsHandlerStub::getLocalLimitationsSchedule(unsigned int                   connector_id,
                                                               unsigned int                   duration,
                                                               ocpp::types::ChargingSchedule& schedule)
{
    (void)schedule;
    m_calls["getLocalLimitationsSchedule"] = {{"connector_id", std::to_string(connector_id)}, {"duration", std::to_string(duration)}};
    return false;
}

/** @copydoc bool IChargePointEventsHandler::resetRequested(ocpp::types::ResetType) */
bool ChargePointEventsHandlerStub::resetRequested(ocpp::types::ResetType reset_type)
{
    m_calls["resetRequested"] = {{"reset_type", ResetTypeHelper.toString(reset_type)}};
    return callResult("resetRequested");
}

/** @copydoc ocpp::types::UnlockStatus IChargePointEventsHandler::unlockConnectorRequested(unsigned int) */
ocpp::types::UnlockStatus ChargePointEventsHandlerStub::unlockConnectorRequested(unsigned int connector_id)
{
    m_calls["unlockConnectorRequested"] = {{"connector_id", std::to_string(connector_id)}};
    return m_unlock_status;
}

/** @copydoc std::string IChargePointEventsHandler::getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                       const ocpp::types::Optional<ocpp::types::DateTime>&) */
std::string ChargePointEventsHandlerStub::getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                                         const ocpp::types::Optional<ocpp::types::DateTime>& stop_time)
{
    m_calls["getDiagnostics"] = {{"start_time", (start_time.isSet() ? start_time.value().str() : "not set")},
                                 {"stop_time", (stop_time.isSet() ? stop_time.value().str() : "not set")}};
    return m_diag_file;
}

/** @copydoc std::string IChargePointEventsHandler::updateFirmwareRequested() */
std::string ChargePointEventsHandlerStub::updateFirmwareRequested()
{
    m_calls["updateFirmwareRequested"] = {{}};
    return m_diag_file;
}

/** @copydoc void IChargePointEventsHandler::installFirmware() */
void ChargePointEventsHandlerStub::installFirmware(const std::string& firmware_file)
{
    m_calls["installFirmware"] = {{"firmware_file", firmware_file}};
}

/** @copydoc bool IChargePointEventsHandler::uploadFile(const std::string&, const std::string&) */
bool ChargePointEventsHandlerStub::uploadFile(const std::string& file, const std::string& url)
{
    m_calls["uploadFile"] = {{"file", file}, {"url", url}};
    return callResult("uploadFile");
}

/** @copydoc bool IChargePointEventsHandler::downloadFile(const std::string&, const std::string&) */
bool ChargePointEventsHandlerStub::downloadFile(const std::string& url, const std::string& file)
{
    m_calls["downloadFile"] = {{"url", url}, {"file", file}};
    return callResult("downloadFile");
}

// Security extensions

/** @copydoc ocpp::types::CertificateStatusEnumType IChargePointEventsHandler::caCertificateReceived(ocpp::types::CertificateUseEnumType,
                                                                                                     const ocpp::x509::Certificate&) */
ocpp::types::CertificateStatusEnumType ChargePointEventsHandlerStub::caCertificateReceived(ocpp::types::CertificateUseEnumType type,
                                                                                           const ocpp::x509::Certificate&      certificate)
{
    m_calls["caCertificateReceived"] = {{"type", CertificateUseEnumTypeHelper.toString(type)}, {"certificate", certificate.pem()}};
    return CertificateStatusEnumType::Accepted;
}

/** @copydoc bool IChargePointEventsHandler::chargePointCertificateReceived(const ocpp::x509::Certificate&) */
bool ChargePointEventsHandlerStub::chargePointCertificateReceived(const ocpp::x509::Certificate& certificate)
{
    m_calls["chargePointCertificateReceived"] = {{"certificate", certificate.pem()}};
    return m_call_results["chargePointCertificateReceived"];
}

/** @copydoc ocpp::types::DeleteCertificateStatusEnumType IChargePointEventsHandler::deleteCertificate(ocpp::types::HashAlgorithmEnumType,
                                                                                                           const std::string&,
                                                                                                           const std::string&,
                                                                                                           const std::string&) */
ocpp::types::DeleteCertificateStatusEnumType ChargePointEventsHandlerStub::deleteCertificate(
    ocpp::types::HashAlgorithmEnumType hash_algorithm,
    const std::string&                 issuer_name_hash,
    const std::string&                 issuer_key_hash,
    const std::string&                 serial_number)
{
    m_calls["deleteCertificate"] = {{"hash_algorithm", HashAlgorithmEnumTypeHelper.toString(hash_algorithm)},
                                    {"issuer_name_hash", issuer_name_hash},
                                    {"issuer_key_hash", issuer_key_hash},
                                    {"serial_number", serial_number}};
    return DeleteCertificateStatusEnumType::Accepted;
}

/** @copydoc void IChargePointEventsHandler::generateCsr(std::string&) */
void ChargePointEventsHandlerStub::generateCsr(std::string& csr)
{
    (void)csr;
    m_calls["generateCsr"] = {{}};
}

/** @copydoc void IChargePointEventsHandler::getInstalledCertificates(ocpp::types::CertificateUseEnumType,
 *                                                                    std::vector<ocpp::x509::Certificate>&) */
void ChargePointEventsHandlerStub::getInstalledCertificates(ocpp::types::CertificateUseEnumType   type,
                                                            std::vector<ocpp::x509::Certificate>& certificates)
{
    (void)certificates;
    m_calls["getInstalledCertificates"] = {{"type", CertificateUseEnumTypeHelper.toString(type)}};
}

/** @copydoc std::string IChargePointEventsHandler::getLog(ocpp::types::LogEnumType,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&) */
std::string ChargePointEventsHandlerStub::getLog(ocpp::types::LogEnumType                            type,
                                                 const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                                 const ocpp::types::Optional<ocpp::types::DateTime>& stop_time)
{
    m_calls["getLog"] = {{"type", LogEnumTypeHelper.toString(type)},
                         {"start_time", (start_time.isSet() ? start_time.value().str() : "not set")},
                         {"stop_time", (stop_time.isSet() ? stop_time.value().str() : "not set")}};
    return m_diag_file;
}

bool ChargePointEventsHandlerStub::hasCentralSystemCaCertificateInstalled()
{
    return m_call_results["hasCentralSystemCaCertificateInstalled"];
}

/** @copydoc bool IChargePointEventsHandler::hasChargePointCertificateInstalled() */
bool ChargePointEventsHandlerStub::hasChargePointCertificateInstalled()
{
    return m_call_results["hasChargePointCertificateInstalled"];
}

/** @copydoc ocpp::types::UpdateFirmwareStatusEnumType IChargePointEventsHandler::checkFirmwareSigningCertificate(
 *                                            const ocpp::x509::Certificate&) */
ocpp::types::UpdateFirmwareStatusEnumType ChargePointEventsHandlerStub::checkFirmwareSigningCertificate(
    const ocpp::x509::Certificate& signing_certificate)
{
    m_calls["checkFirmwareSigningCertificate"] = {{"signing_certificate", signing_certificate.pem()}};
    return UpdateFirmwareStatusEnumType::Accepted;
}

// ISO 15118 PnC extensions

/** @copydoc bool IChargePointEventsHandler::iso15118CheckEvCertificate(const ocpp::x509::Certificate&) */
bool ChargePointEventsHandlerStub::iso15118CheckEvCertificate(const ocpp::x509::Certificate& certificate)
{
    m_calls["iso15118CheckEvCertificate"] = {{"certificate", certificate.pem()}};
    return true;
}

/** @copydoc bool IChargePointEventsHandler::iso15118ChargePointCertificateReceived(const ocpp::x509::Certificate&) */
bool ChargePointEventsHandlerStub::iso15118ChargePointCertificateReceived(const ocpp::x509::Certificate& certificate)
{
    m_calls["iso15118ChargePointCertificateReceived"] = {{"certificate", certificate.pem()}};
    return true;
}

/** @copydoc ocpp::types::DeleteCertificateStatusEnumType IChargePointEventsHandler::iso15118DeleteCertificate(ocpp::types::HashAlgorithmEnumType,
                                                                                                                   const std::string&,
                                                                                                                   const std::string&,
                                                                                                                   const std::string&) */
ocpp::types::DeleteCertificateStatusEnumType ChargePointEventsHandlerStub::iso15118DeleteCertificate(
    ocpp::types::HashAlgorithmEnumType hash_algorithm,
    const std::string&                 issuer_name_hash,
    const std::string&                 issuer_key_hash,
    const std::string&                 serial_number)
{
    m_calls["iso15118ChargePointCertificateReceived"] = {{"hash_algorithm", HashAlgorithmEnumTypeHelper.toString(hash_algorithm)},
                                                         {"issuer_name_hash", issuer_name_hash},
                                                         {"issuer_key_hash", issuer_key_hash},
                                                         {"serial_number", serial_number}};
    return DeleteCertificateStatusEnumType::Accepted;
}

/** @copydoc void IChargePointEventsHandler::iso15118GetInstalledCertificates(
                                    bool,
                                    bool,
                                    bool,
                                    bool,
                                    std::vector<std::tuple<GetCertificateIdUseEnumType, Certificate, std::vector<Certificate>>>&) */
void ChargePointEventsHandlerStub::iso15118GetInstalledCertificates(
    bool v2g_root_certificate,
    bool mo_root_certificate,
    bool v2g_certificate_chain,
    bool oem_root_certificate,
    std::vector<std::tuple<ocpp::types::GetCertificateIdUseEnumType, ocpp::x509::Certificate, std::vector<ocpp::x509::Certificate>>>&
        certificates)
{
    (void)certificates;
    m_calls["iso15118GetInstalledCertificates"] = {{"v2g_root_certificate", std::to_string(v2g_root_certificate)},
                                                   {"mo_root_certificate", std::to_string(mo_root_certificate)},
                                                   {"v2g_certificate_chain", std::to_string(v2g_certificate_chain)},
                                                   {"oem_root_certificate", std::to_string(oem_root_certificate)}};
}

/** @copydoc ocpp::types::InstallCertificateStatusEnumType IChargePointEventsHandler::iso15118CertificateReceived(
                                    ocpp::types::InstallCertificateUseEnumType type,
                                    const ocpp::x509::Certificate&) */
ocpp::types::InstallCertificateStatusEnumType ChargePointEventsHandlerStub::iso15118CertificateReceived(
    ocpp::types::InstallCertificateUseEnumType type, const ocpp::x509::Certificate& certificate)
{
    m_calls["iso15118CertificateReceived"] = {{"type", InstallCertificateUseEnumTypeHelper.toString(type)},
                                              {"certificate", certificate.pem()}};
    return InstallCertificateStatusEnumType::Accepted;
}

/** @copydoc void IChargePointEventsHandler::iso15118GenerateCsr(std::string&) */
void ChargePointEventsHandlerStub::iso15118GenerateCsr(std::string& csr)
{
    m_calls["iso15118GenerateCsr"] = {{"csr", csr}};
}

/** @brief Indicate if a method has been called and returns the parameters used for the call */
bool ChargePointEventsHandlerStub::methodCalled(const std::string& method_name, std::map<std::string, std::string>& params)
{
    bool ret  = false;
    auto iter = m_calls.find(method_name);
    if (iter != m_calls.end())
    {
        params = iter->second;
        ret    = true;
    }
    return ret;
}

/** @brief Clear the call history */
void ChargePointEventsHandlerStub::clearCalls()
{
    m_calls.clear();
}

/** @brief Get the call result of a method */
bool ChargePointEventsHandlerStub::callResult(const std::string method_name)
{
    bool ret  = false;
    auto iter = m_call_results.find(method_name);
    if (iter != m_call_results.end())
    {
        ret = iter->second;
    }
    return ret;
}

/** @brief Set a meter value */
void ChargePointEventsHandlerStub::setMeterValue(unsigned int                                                                 connector_id,
                                                 std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>> measurand,
                                                 const ocpp::types::MeterValue&                                               meter_value)
{
    std::stringstream ss;
    ss << MeasurandHelper.toString(measurand.first) << "-";
    ss << (measurand.second.isSet() ? PhaseHelper.toString(measurand.second) : "none");

    auto iter_con = m_meter_values.find(connector_id);
    if (iter_con != m_meter_values.cend())
    {
        iter_con->second[ss.str()] = meter_value;
    }
    else
    {
        m_meter_values[connector_id] = {{ss.str(), meter_value}};
    }
}
