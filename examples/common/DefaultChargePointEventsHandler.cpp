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
#include "CertificateRequest.h"
#include "ChargePointDemoConfig.h"
#include "PrivateKey.h"
#include "Sha2.h"
#include "StringHelpers.h"

#include <fstream>
#include <iostream>

// With MSVC compiler, the system() call returns directly the command's return value
#ifdef _MSC_VER
#define WEXITSTATUS(x) (x)
#endif // _MSC_VER

using namespace std;
using namespace ocpp::types;
using namespace ocpp::x509;

/** @brief Constructor */
DefaultChargePointEventsHandler::DefaultChargePointEventsHandler(ChargePointDemoConfig& config, const std::filesystem::path& working_dir)
    : m_config(config),
      m_chargepoint(nullptr),
      m_working_dir(working_dir),
      m_remote_start_pending(config.ocppConfig().numberOfConnectors()),
      m_remote_stop_pending(m_remote_start_pending.size()),
      m_remote_start_id_tag(m_remote_start_pending.size())
{
    for (unsigned int i = 0; i < m_remote_start_pending.size(); i++)
    {
        m_remote_start_pending[i] = false;
        m_remote_stop_pending[i]  = false;
        m_remote_start_id_tag[i]  = "";
    }
}

/** @brief Destructor */
DefaultChargePointEventsHandler::~DefaultChargePointEventsHandler() { }

/** @copydoc void IChargePointEventsHandler::connectionStateChanged(ocpp::types::RegistrationStatus) */
void DefaultChargePointEventsHandler::connectionFailed(ocpp::types::RegistrationStatus status)
{
    cout << "Connection failed, previous registration status : " << RegistrationStatusHelper.toString(status) << endl;
}

/** @copydoc void IChargePointEventsHandler::connectionStateChanged(bool) */
void DefaultChargePointEventsHandler::connectionStateChanged(bool isConnected)
{
    cout << "Connection state changed : " << isConnected << endl;
}

/** @copydoc void IChargePointEventsHandler::bootNotification(ocpp::types::RegistrationStatus, const ocpp::types::DateTime&) */
void DefaultChargePointEventsHandler::bootNotification(ocpp::types::RegistrationStatus status, const ocpp::types::DateTime& datetime)
{
    cout << "Bootnotification : " << RegistrationStatusHelper.toString(status) << " - " << datetime.str() << endl;
}

/** @copydoc void IChargePointEventsHandler::datetimeReceived(const ocpp::types::DateTime&) */
void DefaultChargePointEventsHandler::datetimeReceived(const ocpp::types::DateTime& datetime)
{
    cout << "Date time received : " << datetime.str() << endl;
}

/** @copydoc AvailabilityStatus IChargePointEventsHandler::changeAvailabilityRequested(unsigned int, ocpp::types::AvailabilityType) */
ocpp::types::AvailabilityStatus DefaultChargePointEventsHandler::changeAvailabilityRequested(unsigned int                  connector_id,
                                                                                             ocpp::types::AvailabilityType availability)
{
    cout << "Change availability requested : " << connector_id << " - " << AvailabilityTypeHelper.toString(availability) << endl;
    return AvailabilityStatus::Accepted;
}

/** @copydoc int IChargePointEventsHandler::getTxStartStopMeterValue(unsigned int) */
int DefaultChargePointEventsHandler::getTxStartStopMeterValue(unsigned int connector_id)
{
    static int value = 0;
    cout << "Get start/stop meter value for connector " << connector_id << endl;
    value += 100;
    return value;
}

/** @copydoc void IChargePointEventsHandler::reservationStarted(unsigned int) */
void DefaultChargePointEventsHandler::reservationStarted(unsigned int connector_id)
{
    cout << "Reservation started on connector " << connector_id << endl;
}

/** @copydoc void IChargePointEventsHandler::reservationEnded(unsigned int, bool) */
void DefaultChargePointEventsHandler::reservationEnded(unsigned int connector_id, bool canceled)
{
    cout << "End of reservation on connector " << connector_id << " (" << (canceled ? "canceled" : "expired") << ")" << endl;
}

/** @copydoc ocpp::types::DataTransferStatus IChargePointEventsHandler::dataTransferRequested(const std::string&,
                                                                                                  const std::string&,
                                                                                                  const std::string&,
                                                                                                  std::string&) */
ocpp::types::DataTransferStatus DefaultChargePointEventsHandler::dataTransferRequested(const std::string& vendor_id,
                                                                                       const std::string& message_id,
                                                                                       const std::string& request_data,
                                                                                       std::string&       response_data)
{
    DataTransferStatus ret = DataTransferStatus::Accepted;
    cout << "Data transfer received : " << vendor_id << " - " << message_id << " - " << request_data << endl;
    if (vendor_id == "vendor_test")
    {
        if (message_id == "message_test")
        {
            if (request_data.empty())
            {
                ret = DataTransferStatus::Rejected;
            }
            else
            {
                response_data = "Hop!";
            }
        }
        else
        {
            ret = DataTransferStatus::UnknownMessageId;
        }
    }
    else
    {
        ret = DataTransferStatus::UnknownVendorId;
    }
    return ret;
}

/** @copydoc bool getMeterValue(unsigned int, const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>&, ocpp::types::MeterValue&) */
bool DefaultChargePointEventsHandler::getMeterValue(
    unsigned int                                                                        connector_id,
    const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
    ocpp::types::MeterValue&                                                            meter_value)
{
    (void)meter_value;

    cout << "getMeterValue : " << connector_id << " - " << MeasurandHelper.toString(measurand.first) << endl;

    return false;
}

/** @copydoc bool IChargePointEventsHandler::remoteStartTransactionRequested(unsigned int, const std::string&) */
bool DefaultChargePointEventsHandler::remoteStartTransactionRequested(unsigned int connector_id, const std::string& id_tag)
{
    bool ret = false;
    cout << "Remote start transaction : " << connector_id << " - " << id_tag << endl;

    if(connector_id > m_config.ocppConfig().numberOfConnectors() || connector_id == 0)
    {
        ret=false;
    }
    else
    {
        m_remote_start_pending[connector_id - 1u] = true;
        m_remote_start_id_tag[connector_id - 1u]  = id_tag;
        ret=true;
    }
    return ret;
}

/** @copydoc bool IChargePointEventsHandler::remoteStopTransactionRequested(unsigned int) */
bool DefaultChargePointEventsHandler::remoteStopTransactionRequested(unsigned int connector_id)
{
    cout << "Remote stop transaction : " << connector_id << endl;
    m_remote_stop_pending[connector_id - 1u] = true;
    return true;
}

/** @copydoc void IChargePointEventsHandler::transactionDeAuthorized(unsigned int) */
void DefaultChargePointEventsHandler::transactionDeAuthorized(unsigned int connector_id)
{
    cout << "Transaction deauthorized on connector : " << connector_id << endl;
}

/** @copydoc bool IChargePointEventsHandler::getLocalLimitationsSchedule(unsigned int, ocpp::types::ChargingSchedule&) */
bool DefaultChargePointEventsHandler::getLocalLimitationsSchedule(unsigned int                   connector_id,
                                                                  unsigned int                   duration,
                                                                  ocpp::types::ChargingSchedule& schedule)
{
    (void)schedule;
    cout << "Local limitations schedule requested : " << connector_id << " - " << duration << endl;
    return false;
}

/** @copydoc bool IChargePointEventsHandler::resetRequested(ocpp::types::ResetType) */
bool DefaultChargePointEventsHandler::resetRequested(ocpp::types::ResetType reset_type)
{
    cout << "Reset requested : " << ResetTypeHelper.toString(reset_type) << endl;
    return true;
}

/** @copydoc ocpp::types::UnlockStatus IChargePointEventsHandler::unlockConnectorRequested(unsigned int) */
ocpp::types::UnlockStatus DefaultChargePointEventsHandler::unlockConnectorRequested(unsigned int connector_id)
{
    cout << "Unlock connector " << connector_id << " requested" << endl;
    return UnlockStatus::Unlocked;
}

/** @copydoc std::string IChargePointEventsHandler::getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                                       const ocpp::types::Optional<ocpp::types::DateTime>&) */
std::string DefaultChargePointEventsHandler::getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                                            const ocpp::types::Optional<ocpp::types::DateTime>& stop_time)
{
    cout << "Get diagnostics" << endl;
    (void)start_time;
    (void)stop_time;

    std::string diag_file = "/tmp/diag.zip";

    std::stringstream ss;
    ss << "zip " << diag_file << " " << m_config.stackConfig().databasePath();
    int err = WEXITSTATUS(system(ss.str().c_str()));
    cout << "Command line : " << ss.str() << " => " << err << endl;

    return diag_file;
}

/** @copydoc std::string IChargePointEventsHandler::updateFirmwareRequested() */
std::string DefaultChargePointEventsHandler::updateFirmwareRequested()
{
    cout << "Firmware update requested" << endl;
    return "/tmp/firmware.tar.gz";
}

/** @copydoc void IChargePointEventsHandler::installFirmware() */
void DefaultChargePointEventsHandler::installFirmware(const std::string& firmware_file)
{
    cout << "Firmware to install : " << firmware_file << endl;
}

/** @copydoc bool IChargePointEventsHandler::uploadFile(const std::string&, const std::string&) */
bool DefaultChargePointEventsHandler::uploadFile(const std::string& file, const std::string& url)
{
    bool ret = true;

    cout << "Uploading " << file << " to " << url << endl;

    std::string connection_url = url;
    std::string params;
    if (connection_url.find("ftp://") == 0)
    {
        // FTP => no specific params
    }
    else if (connection_url.find("ftps://") == 0)
    {
        // FTPS
        params = "--insecure --ssl";
        ocpp::helpers::replace(connection_url, "ftps://", "ftp://", false);
    }
    else if (connection_url.find("http://") == 0)
    {
        // HTTP => no specific params
    }
    else if (connection_url.find("https://") == 0)
    {
        // HTTP
        params = "--insecure";
    }
    else
    {
        // Unsupported protocol
        ret = false;
    }
    if (ret)
    {
        std::stringstream ss;
        ss << "curl --silent " << params << " -T " << file << " " << connection_url;
        int err = WEXITSTATUS(system(ss.str().c_str()));
        cout << "Command line : " << ss.str() << endl;
        ret = (err == 0);
    }

    return ret;
}

/** @copydoc bool IChargePointEventsHandler::downloadFile(const std::string&, const std::string&) */
bool DefaultChargePointEventsHandler::downloadFile(const std::string& url, const std::string& file)
{
    bool ret = true;
    cout << "Downloading from " << url << " to " << file << endl;

    std::string connection_url = url;
    std::string params;
    if (connection_url.find("ftp://") == 0)
    {
        // FTP => no specific params
    }
    else if (connection_url.find("ftps://") == 0)
    {
        // FTPS
        params = "--insecure --ssl";
        ocpp::helpers::replace(connection_url, "ftps://", "ftp://", false);
    }
    else if (connection_url.find("http://") == 0)
    {
        // HTTP => no specific params
    }
    else if (connection_url.find("https://") == 0)
    {
        // HTTP
        params = "--insecure";
    }
    else
    {
        // Unsupported protocol
        ret = false;
    }
    if (ret)
    {
        std::stringstream ss;
        ss << "curl --silent " << params << " -o " << file << " " << connection_url;
        int err = WEXITSTATUS(system(ss.str().c_str()));
        cout << "Command line : " << ss.str() << endl;
        ret = (err == 0);
    }

    return ret;
}

// Security extensions

/** @copydoc ocpp::types::CertificateStatusEnumType IChargePointEventsHandler::caCertificateReceived(ocpp::types::CertificateUseEnumType,
                                                                                                     const ocpp::x509::Certificate&) */
ocpp::types::CertificateStatusEnumType DefaultChargePointEventsHandler::caCertificateReceived(ocpp::types::CertificateUseEnumType type,
                                                                                              const ocpp::x509::Certificate& certificate)
{
    std::string               ca_filename;
    CertificateStatusEnumType ret = CertificateStatusEnumType::Rejected;

    cout << "CA certificate installation requested : type = " << CertificateUseEnumTypeHelper.toString(type)
         << " - certificate subject = " << certificate.subjectString() << endl;

    // Check number of installed certificates
    if (getNumberOfCaCertificateInstalled(true, true, true) < m_config.ocppConfig().certificateStoreMaxLength())
    {
        // Compute SHA256 to generate filename
        Sha2 sha256;
        sha256.compute(certificate.pem().c_str(), certificate.pem().size());

        if (type == CertificateUseEnumType::ManufacturerRootCertificate)
        {
            // Manufacturer => generate a filename to add the new CA

            std::stringstream name;
            name << "fw_" << sha256.resultString() << ".pem";
            ca_filename = (m_working_dir / name.str()).string();
        }
        else
        {
            // Central System => Check AdditionalRootCertificateCheck configuration key

            if (m_config.ocppConfig().additionalRootCertificateCheck() && (getNumberOfCaCertificateInstalled(false, true, false) == 0))
            {
                // Additionnal checks :
                // - only 1 CA certificate allowed
                // - new certificate must be signed by the old one

                // TODO :)
            }

            std::stringstream name;
            name << "cs_" << sha256.resultString() << ".pem";
            ca_filename = (m_working_dir / name.str()).string();
        }

        // Check if the certificate must be saved
        if (!ca_filename.empty())
        {
            if (certificate.toFile(ca_filename))
            {
                ret = CertificateStatusEnumType::Accepted;
                cout << "Certificate saved : " << ca_filename << endl;

                if (type == CertificateUseEnumType::CentralSystemRootCertificate)
                {
                    // Use the new certificate
                    m_config.setStackConfigValue("TlsServerCertificateCa", ca_filename);
                    if (m_chargepoint)
                    {
                        m_chargepoint->reconnect();
                    }
                }
            }
            else
            {
                ret = CertificateStatusEnumType::Failed;
                cout << "Unable to save certificate : " << ca_filename << endl;
            }
        }
    }
    else
    {
        cout << "Maximum number of certificates reached" << endl;
    }

    return ret;
}

/** @copydoc bool IChargePointEventsHandler::chargePointCertificateReceived(const ocpp::x509::Certificate&) */
bool DefaultChargePointEventsHandler::chargePointCertificateReceived(const ocpp::x509::Certificate& certificate)
{
    std::string ca_filename;
    bool        ret = false;

    cout << "Charge point certificate installation requested : certificate subject = " << certificate.subjectString() << endl;

    // Compute SHA256 to generate filename
    Sha2 sha256;
    sha256.compute(certificate.pem().c_str(), certificate.pem().size());

    std::stringstream name;
    name << "cp_" << sha256.resultString() << ".pem";
    std::string cert_filename = (m_working_dir / name.str()).string();

    // Save certificate
    if (certificate.toFile(cert_filename))
    {
        cout << "Certificate saved : " << cert_filename << endl;

        // Retrieve and save the corresponding key/pair with the new certificate
        std::string cert_key_filename = cert_filename + ".key";
        std::filesystem::copy("/tmp/charge_point_key.key", cert_key_filename);

        // Use the new certificate
        m_config.setStackConfigValue("TlsClientCertificate", cert_filename);
        m_config.setStackConfigValue("TlsClientCertificatePrivateKey", cert_key_filename);
        if (m_chargepoint)
        {
            m_chargepoint->reconnect();
        }

        ret = true;
    }
    else
    {
        cout << "Unable to save certificate : " << cert_filename << endl;
    }

    return ret;
}

/** @copydoc ocpp::types::DeleteCertificateStatusEnumType IChargePointEventsHandler::deleteCertificate(ocpp::types::HashAlgorithmEnumType,
                                                                                                           const std::string&,
                                                                                                           const std::string&,
                                                                                                           const std::string&) */
ocpp::types::DeleteCertificateStatusEnumType DefaultChargePointEventsHandler::deleteCertificate(
    ocpp::types::HashAlgorithmEnumType hash_algorithm,
    const std::string&                 issuer_name_hash,
    const std::string&                 issuer_key_hash,
    const std::string&                 serial_number)
{
    DeleteCertificateStatusEnumType ret = DeleteCertificateStatusEnumType::NotFound;

    cout << "CA certificate deletion requested : hash = " << HashAlgorithmEnumTypeHelper.toString(hash_algorithm)
         << " - serial number = " << serial_number << endl;

    // Prepare for hash computation
    Sha2::Type sha_type;
    if (hash_algorithm == HashAlgorithmEnumType::SHA256)
    {
        sha_type = Sha2::Type::SHA256;
    }
    else if (hash_algorithm == HashAlgorithmEnumType::SHA384)
    {
        sha_type = Sha2::Type::SHA384;
    }
    else
    {
        sha_type = Sha2::Type::SHA512;
    }

    // Look for installed certificates
    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if ((ocpp::helpers::startsWith(filename, "fw_") || ocpp::helpers::startsWith(filename, "cs_") ||
                 ocpp::helpers::startsWith(filename, "iso_")) &&
                ocpp::helpers::endsWith(filename, ".pem"))
            {
                Certificate certificate(dir_entry.path());
                if (certificate.isValid() && certificate.serialNumberHexString() == serial_number)
                {
                    Sha2 sha(sha_type);
                    sha.compute(certificate.issuerString().c_str(), certificate.issuerString().size());
                    if (issuer_name_hash == sha.resultString())
                    {
                        sha.compute(&certificate.publicKey()[0], certificate.publicKey().size());
                        if (issuer_key_hash == sha.resultString())
                        {
                            if ((filename == m_config.stackConfig().tlsServerCertificateCa()) || !std::filesystem::remove(dir_entry.path()))
                            {
                                ret = DeleteCertificateStatusEnumType::Failed;
                            }
                            else
                            {
                                ret = DeleteCertificateStatusEnumType::Accepted;
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

/** @copydoc void IChargePointEventsHandler::generateCsr(std::string&) */
void DefaultChargePointEventsHandler::generateCsr(std::string& csr)
{
    cout << "Generate CSR requested" << endl;

    // Generata a new public/private key pair
    PrivateKey private_key(PrivateKey::Type::EC,
                           static_cast<unsigned int>(PrivateKey::Curve::PRIME256_V1),
                           m_config.stackConfig().tlsClientCertificatePrivateKeyPassphrase());
    private_key.privateToFile("/tmp/charge_point_key.key");

    // Generate the CSR
    CertificateRequest::Subject subject;
    subject.country           = m_config.stackConfig().clientCertificateRequestSubjectCountry();
    subject.state             = m_config.stackConfig().clientCertificateRequestSubjectState();
    subject.location          = m_config.stackConfig().clientCertificateRequestSubjectLocation();
    subject.organization      = m_config.ocppConfig().cpoName();
    subject.organization_unit = m_config.stackConfig().clientCertificateRequestSubjectOrganizationUnit();
    subject.common_name       = m_config.stackConfig().chargePointSerialNumber();
    subject.email_address     = m_config.stackConfig().clientCertificateRequestSubjectEmail();
    CertificateRequest certificate_request(subject, private_key);
    csr = certificate_request.pem();
}

/** @copydoc void IChargePointEventsHandler::getInstalledCertificates(ocpp::types::CertificateUseEnumType,
 *                                                                    std::vector<ocpp::x509::Certificate>&) */
void DefaultChargePointEventsHandler::getInstalledCertificates(ocpp::types::CertificateUseEnumType   type,
                                                               std::vector<ocpp::x509::Certificate>& certificates)
{
    cout << "Get installed CA certificates requested : type = " << CertificateUseEnumTypeHelper.toString(type) << endl;

    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if (type == CertificateUseEnumType::ManufacturerRootCertificate)
            {
                if (ocpp::helpers::startsWith(filename, "fw_") && ocpp::helpers::endsWith(filename, ".pem"))
                {
                    certificates.emplace_back(dir_entry.path());
                }
            }
            else
            {
                if (ocpp::helpers::startsWith(filename, "cs_") && ocpp::helpers::endsWith(filename, ".pem"))
                {
                    certificates.emplace_back(dir_entry.path());
                }
            }
        }
    }
}

/** @copydoc std::string IChargePointEventsHandler::getLog(ocpp::types::LogEnumType,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&,
                                                           const ocpp::types::Optional<ocpp::types::DateTime>&) */
std::string DefaultChargePointEventsHandler::getLog(ocpp::types::LogEnumType                            type,
                                                    const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                                    const ocpp::types::Optional<ocpp::types::DateTime>& stop_time)
{
    cout << "Get log : type = " << LogEnumTypeHelper.toString(type) << endl;
    (void)start_time;
    (void)stop_time;

    std::string log_file = "";
    if (type == LogEnumType::SecurityLog)
    {
        // Security logs :
        // if security logs are handled by the Open OCPP stack, just return a path where
        // the stack can generate the log file, otherwise you'll have to generate your
        // own log file as for the diagnostics logs
        if (m_config.stackConfig().securityLogMaxEntriesCount() > 0)
        {
            // The stack will generate the log file into the follwing folder
            log_file = "/tmp/";
        }
        else
        {
            // You'll have to implement the log file generation and provide the path
            // to the generated file
        }
    }
    else
    {
        // Dianostic logs
        log_file = "/tmp/diag.zip";

        std::stringstream ss;
        ss << "zip " << log_file << " " << m_config.stackConfig().databasePath();
        int err = WEXITSTATUS(system(ss.str().c_str()));
        cout << "Command line : " << ss.str() << " => " << err << endl;
    }

    return log_file;
}

bool DefaultChargePointEventsHandler::hasCentralSystemCaCertificateInstalled()
{
    // A better implementation would also check the validity dates of the certificates
    return ((getNumberOfCaCertificateInstalled(false, true, false) != 0) && (!m_config.stackConfig().tlsServerCertificateCa().empty()));
}

/** @copydoc bool IChargePointEventsHandler::hasChargePointCertificateInstalled() */
bool DefaultChargePointEventsHandler::hasChargePointCertificateInstalled()
{
    // A better implementation would also check the validity dates of the certificates
    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if (ocpp::helpers::startsWith(filename, "cp_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                std::string certificate_key = dir_entry.path().string() + ".key";
                if (std::filesystem::exists(certificate_key) && !m_config.stackConfig().tlsClientCertificate().empty() &&
                    !m_config.stackConfig().tlsClientCertificatePrivateKey().empty())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

/** @copydoc ocpp::types::UpdateFirmwareStatusEnumType IChargePointEventsHandler::checkFirmwareSigningCertificate(
 *                                            const ocpp::x509::Certificate&) */
ocpp::types::UpdateFirmwareStatusEnumType DefaultChargePointEventsHandler::checkFirmwareSigningCertificate(
    const ocpp::x509::Certificate& signing_certificate)
{
    UpdateFirmwareStatusEnumType ret = UpdateFirmwareStatusEnumType::InvalidCertificate;

    cout << "Check of firmware signing certificate requested : subject = " << signing_certificate.subjectString()
         << " - issuer = " << signing_certificate.issuerString() << endl;

    // Load all installed Manufacturer CA certificates
    std::vector<Certificate> ca_certificates;
    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if (ocpp::helpers::startsWith(filename, "fw_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                ca_certificates.emplace_back(dir_entry.path());
            }
        }
    }
    if (!ca_certificates.empty())
    {
        // Check signing certificate
        for (const auto& cer : ca_certificates)
        {
            if (signing_certificate.verify(cer.certificateChain()))
            {
                ret = UpdateFirmwareStatusEnumType::Accepted;
                break;
            }
        }
    }
    else
    {
        cout << "No manufacturer CA installed" << endl;
    }

    return ret;
}

// ISO 15118 PnC extensions

/** @copydoc bool IChargePointEventsHandler::iso15118CheckEvCertificate(const ocpp::x509::Certificate&) */
bool DefaultChargePointEventsHandler::iso15118CheckEvCertificate(const ocpp::x509::Certificate& certificate)
{
    bool ret = false;

    cout << "ISO15118 EV certificate verification requested : certificate subject = " << certificate.subjectString() << endl;

    // Look for MO certificates
    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if (ocpp::helpers::startsWith(filename, "iso_mo_root_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                Certificate mo_cert(dir_entry.path());
                if (certificate.verify(mo_cert.certificateChain()))
                {
                    cout << "Validated against certificate : " << mo_cert.subjectString() << endl;
                    ret = true;
                    break;
                }
            }
        }
    }

    cout << "EV certificate validated : " << (ret ? "yes" : "no") << endl;

    return ret;
}

/** @copydoc bool IChargePointEventsHandler::iso15118ChargePointCertificateReceived(const ocpp::x509::Certificate&) */
bool DefaultChargePointEventsHandler::iso15118ChargePointCertificateReceived(const ocpp::x509::Certificate& certificate)
{
    std::string ca_filename;
    bool        ret = false;

    cout << "ISO15118 Charge point certificate installation requested : certificate subject = " << certificate.subjectString() << endl;

    // Compute SHA256 to generate filename
    Sha2 sha256;
    sha256.compute(certificate.pem().c_str(), certificate.pem().size());

    std::stringstream name;
    name << "iso_cp_" << sha256.resultString() << ".pem";
    std::string cert_filename = (m_working_dir / name.str()).string();

    // Save certificate
    if (certificate.toFile(cert_filename))
    {
        // Retrieve and save the corresponding key/pair with the new certificate
        std::string cert_key_filename = cert_filename + ".key";
        std::filesystem::copy("/tmp/charge_point_key.key", cert_key_filename);

        cout << "Certificate saved : " << cert_filename << endl;
        ret = true;
    }
    else
    {
        cout << "Unable to save certificate : " << cert_filename << endl;
    }

    return ret;
}

/** @copydoc ocpp::types::DeleteCertificateStatusEnumType IChargePointEventsHandler::iso15118DeleteCertificate(ocpp::types::HashAlgorithmEnumType,
                                                                                                                   const std::string&,
                                                                                                                   const std::string&,
                                                                                                                   const std::string&) */
ocpp::types::DeleteCertificateStatusEnumType DefaultChargePointEventsHandler::iso15118DeleteCertificate(
    ocpp::types::HashAlgorithmEnumType hash_algorithm,
    const std::string&                 issuer_name_hash,
    const std::string&                 issuer_key_hash,
    const std::string&                 serial_number)
{
    cout << "ISO15118 certificate deletion requested : hash = " << HashAlgorithmEnumTypeHelper.toString(hash_algorithm)
         << " - serial number = " << serial_number << endl;
    return deleteCertificate(hash_algorithm, issuer_name_hash, issuer_key_hash, serial_number);
}

/** @copydoc void IChargePointEventsHandler::iso15118GetInstalledCertificates(
                                    bool,
                                    bool,
                                    bool,
                                    std::vector<std::tuple<GetCertificateIdUseEnumType, Certificate, std::vector<Certificate>>>&) */
void DefaultChargePointEventsHandler::iso15118GetInstalledCertificates(
    bool v2g_root_certificate,
    bool mo_root_certificate,
    bool v2g_certificate_chain,
    std::vector<std::tuple<ocpp::types::GetCertificateIdUseEnumType, ocpp::x509::Certificate, std::vector<ocpp::x509::Certificate>>>&
        certificates)
{
    cout << "ISO15118 get installed certificates requested : v2g_root_certificate = " << (v2g_root_certificate ? "yes" : "no")
         << " - mo_root_certificate = " << (mo_root_certificate ? "yes" : "no")
         << " - v2g_certificate_chain = " << (v2g_certificate_chain ? "yes" : "no") << endl;

    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if (v2g_root_certificate)
            {
                if (ocpp::helpers::startsWith(filename, "iso_v2g_root_") && ocpp::helpers::endsWith(filename, ".pem"))
                {
                    auto tuple = std::make_tuple(GetCertificateIdUseEnumType::V2GRootCertificate,
                                                 Certificate(dir_entry.path()),
                                                 std::vector<ocpp::x509::Certificate>());
                    certificates.emplace_back(std::move(tuple));
                }
            }
            if (mo_root_certificate)
            {
                if (ocpp::helpers::startsWith(filename, "iso_mo_root_") && ocpp::helpers::endsWith(filename, ".pem"))
                {
                    auto tuple = std::make_tuple(GetCertificateIdUseEnumType::MORootCertificate,
                                                 Certificate(dir_entry.path()),
                                                 std::vector<ocpp::x509::Certificate>());
                    certificates.emplace_back(std::move(tuple));
                }
            }
            if (v2g_certificate_chain)
            {
                if (ocpp::helpers::startsWith(filename, "iso_v2g_chain_") && ocpp::helpers::endsWith(filename, ".pem"))
                {
                    auto tuple = std::make_tuple(GetCertificateIdUseEnumType::V2GCertificateChain,
                                                 Certificate(dir_entry.path()),
                                                 std::vector<ocpp::x509::Certificate>());
                    certificates.emplace_back(std::move(tuple));
                }
            }
        }
    }
}

/** @copydoc ocpp::types::InstallCertificateStatusEnumType IChargePointEventsHandler::iso15118CertificateReceived(
                                    ocpp::types::InstallCertificateUseEnumType type,
                                    const ocpp::x509::Certificate&) */
ocpp::types::InstallCertificateStatusEnumType DefaultChargePointEventsHandler::iso15118CertificateReceived(
    ocpp::types::InstallCertificateUseEnumType type, const ocpp::x509::Certificate& certificate)
{
    std::string                      cert_filename;
    InstallCertificateStatusEnumType ret = InstallCertificateStatusEnumType::Rejected;

    cout << "ISO15118 certificate installation requested : type = " << InstallCertificateUseEnumTypeHelper.toString(type)
         << " - certificate subject = " << certificate.subjectString() << endl;

    // Check number of installed certificates
    if (getNumberOfCaCertificateInstalled(true, true, true) < m_config.ocppConfig().certificateStoreMaxLength())
    {
        // Compute SHA256 to generate filename
        Sha2 sha256;
        sha256.compute(certificate.pem().c_str(), certificate.pem().size());

        if (type == InstallCertificateUseEnumType::V2GRootCertificate)
        {
            // V2 root certificate
            std::stringstream name;
            name << "iso_v2g_root_" << sha256.resultString() << ".pem";
            cert_filename = (m_working_dir / name.str()).string();
        }
        else
        {
            // MO root certificate
            std::stringstream name;
            name << "iso_mo_root_" << sha256.resultString() << ".pem";
            cert_filename = (m_working_dir / name.str()).string();
        }

        // Save certificate
        if (certificate.toFile(cert_filename))
        {
            ret = InstallCertificateStatusEnumType::Accepted;
            cout << "Certificate saved : " << cert_filename << endl;
        }
        else
        {
            ret = InstallCertificateStatusEnumType::Failed;
            cout << "Unable to save certificate : " << cert_filename << endl;
        }
    }
    else
    {
        cout << "Maximum number of certificates reached" << endl;
    }

    return ret;
}

/** @copydoc void IChargePointEventsHandler::iso15118GenerateCsr(std::string&) */
void DefaultChargePointEventsHandler::iso15118GenerateCsr(std::string& csr)
{
    cout << "Generate ISO15118 CSR requested" << endl;
    generateCsr(csr);
}

/** @brief Get the number of installed CA certificates */
unsigned int DefaultChargePointEventsHandler::getNumberOfCaCertificateInstalled(bool manufacturer, bool central_system, bool iso15118)
{
    unsigned int count = 0;
    for (auto const& dir_entry : std::filesystem::directory_iterator{m_working_dir})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename().string();
            if (manufacturer && ocpp::helpers::startsWith(filename, "fw_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                count++;
            }
            if (central_system && ocpp::helpers::startsWith(filename, "cs_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                count++;
            }
            if (iso15118 && ocpp::helpers::startsWith(filename, "iso_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                count++;
            }
        }
    }
    return count;
}
