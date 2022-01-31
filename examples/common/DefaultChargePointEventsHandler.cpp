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
#include "Sha2.h"
#include "String.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace ocpp::types;
using namespace ocpp::websockets;

/** @brief Constructor */
DefaultChargePointEventsHandler::DefaultChargePointEventsHandler(ChargePointDemoConfig& config)
    : m_config(config),
      m_chargepoint(nullptr),
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

/** @copydoc unsigned int IChargePointEventsHandler::getTxStartStopMeterValue(unsigned int) */
unsigned int DefaultChargePointEventsHandler::getTxStartStopMeterValue(unsigned int connector_id)
{
    static unsigned int value = 0;
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
    cout << "Remote start transaction : " << connector_id << " - " << id_tag << endl;
    m_remote_start_pending[connector_id - 1u] = true;
    m_remote_start_id_tag[connector_id - 1u]  = id_tag;
    return true;
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
    system(ss.str().c_str());

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
                                                                                                         const ocpp::websockets::Certificate&) */
ocpp::types::CertificateStatusEnumType DefaultChargePointEventsHandler::caCertificateReceived(
    ocpp::types::CertificateUseEnumType type, const ocpp::websockets::Certificate& certificate)
{
    std::string               ca_filename;
    CertificateStatusEnumType ret = CertificateStatusEnumType::Rejected;

    cout << "CA certificate installation requested : type = " << CertificateUseEnumTypeHelper.toString(type)
         << " - certificate subject = " << certificate.subjectString() << endl;

    // Check number of installed certificates
    if (getNumberOfCaCertificateInstalled(true, true) < m_config.ocppConfig().certificateStoreMaxLength())
    {
        // Compute SHA256 to generate filename
        Sha2 sha256;
        sha256.compute(certificate.pem().c_str(), certificate.pem().size());

        if (type == CertificateUseEnumType::ManufacturerRootCertificate)
        {
            // Manufacturer => generate a filename to add the new CA

            std::stringstream name;
            name << "fw_" << sha256.resultString() << ".pem";
            ca_filename = name.str();
        }
        else
        {
            // Central System => Check AdditionalRootCertificateCheck configuration key

            if (m_config.ocppConfig().additionalRootCertificateCheck() && (getNumberOfCaCertificateInstalled(false, true) == 0))
            {
                // Additionnal checks :
                // - only 1 CA certificate allowed
                // - new certificate must be signed by the old one

                // TODO :)
            }

            std::stringstream name;
            name << "cs_" << sha256.resultString() << ".pem";
            ca_filename = name.str();
        }

        // Check if the certificate must be saved
        if (!ca_filename.empty())
        {
            std::fstream certificate_file(ca_filename, certificate_file.out);
            if (certificate_file.is_open())
            {
                certificate_file << certificate.pem();
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
    for (auto const& dir_entry : std::filesystem::directory_iterator{std::filesystem::current_path()})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename();
            if ((ocpp::helpers::startsWith(filename, "fw_") || ocpp::helpers::startsWith(filename, "cs_")) &&
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
    std::string generate_params_cmd_line = "openssl ecparam -name prime256v1 -out /tmp/charge_point_key.param";
    system(generate_params_cmd_line.c_str());
    std::string generate_key_cmd_line = "openssl ecparam -in /tmp/charge_point_key.param -genkey -noout -out /tmp/charge_point_key.key";
    system(generate_key_cmd_line.c_str());

    // Create configuration file to generate the CSR
    std::stringstream csr_config;
    csr_config << R"([req]
                     distinguished_name	= req_distinguished_name

                     # Stop confirmation prompts. All information is contained below.
                     prompt			= no

                     # The extensions to add to a certificate request
                     x509_extensions = v3_ca

                     [req_distinguished_name]
                     countryName =            FR
                     stateOrProvinceName =    Savoie
                     localityName =           Chambery
                     organizationName =)"
               << m_config.ocppConfig().cpoName() << R"(
                     organizationalUnitName = Open OCPP Charge Points
                     commonName =)"
               << m_config.stackConfig().chargePointSerialNumber() << R"(
                     emailAddress =           charge.point@open-ocpp.org
 
                     [v3_ca]
                     basicConstraints = CA:FALSE
                     subjectAltName = @alt_names

                     [alt_names]
                     DNS.1 = localhost
                     DNS.2 = IP:127.0.0.1)";
    std::fstream csr_config_file("/tmp/charge_point_csr.cnf", csr_config_file.out);
    if (csr_config_file.is_open())
    {
        csr_config_file << csr_config.str();
        csr_config_file.close();
    }

    // Generate the CSR
    std::string generate_csr_cmd_line = "openssl req -new -sha256 -key /tmp/charge_point_key.key -extensions v3_ca -config "
                                        "/tmp/charge_point_csr.cnf -out /tmp/charge_point.csr";
    system(generate_csr_cmd_line.c_str());

    // Read generated CSR file
    std::fstream csr_file("/tmp/charge_point.csr", csr_config_file.in | csr_config_file.binary | csr_config_file.ate);
    if (csr_file.is_open())
    {
        auto filesize = csr_file.tellg();
        csr_file.seekg(0, csr_file.beg);
        csr.resize(filesize);
        csr_file.read(&csr[0], filesize);

        csr_file.close();
    }
}

/** @copydoc void IChargePointEventsHandler::getInstalledCertificates(ocpp::types::CertificateUseEnumType,
 *                                                                    std::vector<ocpp::websockets::Certificate>&) */
void DefaultChargePointEventsHandler::getInstalledCertificates(ocpp::types::CertificateUseEnumType         type,
                                                               std::vector<ocpp::websockets::Certificate>& certificates)
{
    cout << "Get installed CA certificates requested : type = " << CertificateUseEnumTypeHelper.toString(type) << endl;

    for (auto const& dir_entry : std::filesystem::directory_iterator{std::filesystem::current_path()})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename();
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
        system(ss.str().c_str());
    }

    return log_file;
}

bool DefaultChargePointEventsHandler::hasCentralSystemCaCertificateInstalled()
{
    return ((getNumberOfCaCertificateInstalled(false, true) != 0) && (!m_config.stackConfig().tlsServerCertificateCa().empty()));
}

/** @brief Get the number of installed CA certificates */
unsigned int DefaultChargePointEventsHandler::getNumberOfCaCertificateInstalled(bool manufacturer, bool central_system)
{
    unsigned int count = 0;
    for (auto const& dir_entry : std::filesystem::directory_iterator{std::filesystem::current_path()})
    {
        if (!dir_entry.is_directory())
        {
            std::string filename = dir_entry.path().filename();
            if (manufacturer && ocpp::helpers::startsWith(filename, "fw_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                count++;
            }
            if (central_system && ocpp::helpers::startsWith(filename, "cs_") && ocpp::helpers::endsWith(filename, ".pem"))
            {
                count++;
            }
        }
    }
    return count;
}
