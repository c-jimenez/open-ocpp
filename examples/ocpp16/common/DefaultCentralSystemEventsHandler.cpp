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
using namespace ocpp::x509;

/** @brief Constructor */
DefaultCentralSystemEventsHandler::DefaultCentralSystemEventsHandler(std::filesystem::path iso_v2g_root_ca,
                                                                     std::filesystem::path iso_mo_root_ca,
                                                                     bool                  set_pending_status)
    : m_chargepoints_mutex(),
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
void DefaultCentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
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
    std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
{
    std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
    m_pending_chargepoints[chargepoint->identifier()] = chargepoint;
}

/** @brief Add a charge point to the accepted list */
void DefaultCentralSystemEventsHandler::addAcceptedChargePoint(
    std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
{
    std::lock_guard<std::mutex> lock(m_chargepoints_mutex);
    m_accepted_chargepoints[chargepoint->identifier()] = chargepoint;
}

/** @brief Constructor */
DefaultCentralSystemEventsHandler::ChargePointRequestHandler::ChargePointRequestHandler(
    DefaultCentralSystemEventsHandler& event_handler, std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint>& chargepoint)
    : m_event_handler(event_handler), m_chargepoint(chargepoint)
{
    m_chargepoint->registerHandler(*this);
}

/** @brief Destructor */
DefaultCentralSystemEventsHandler::ChargePointRequestHandler::~ChargePointRequestHandler() { }

// IChargePointRequestHandler interface

/** @copydoc void IChargePointRequestHandler::disconnected() */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::disconnected()
{
    cout << "[" << m_chargepoint->identifier() << "] - Disconnected" << endl;
    m_event_handler.removeChargePoint(m_chargepoint->identifier());
}

/** @copydoc ocpp::types::IdTagInfo IChargePointRequestHandler::authorize(const std::string&) */
ocpp::types::IdTagInfo DefaultCentralSystemEventsHandler::ChargePointRequestHandler::authorize(const std::string& id_tag)
{
    cout << "[" << m_chargepoint->identifier() << "] - Authorize : " << id_tag << endl;

    IdTagInfo tag_info;
    tag_info.status     = AuthorizationStatus::Accepted;
    tag_info.expiryDate = DateTime(DateTime::now().timestamp() + 3600);
    return tag_info;
}

/** @copydoc ocpp::types::RegistrationStatus IChargePointRequestHandler::bootNotification(const std::string&,
                                                                                          const std::string&,
                                                                                          const std::string&,
                                                                                          const std::string&,
                                                                                          const std::string&,
                                                                                          const std::string&,
                                                                                          const std::string&,
                                                                                          const std::string&) */
ocpp::types::RegistrationStatus DefaultCentralSystemEventsHandler::ChargePointRequestHandler::bootNotification(
    const std::string& model,
    const std::string& serial_number,
    const std::string& vendor,
    const std::string& firmware_version,
    const std::string& iccid,
    const std::string& imsi,
    const std::string& meter_serial_number,
    const std::string& meter_type)
{
    (void)iccid;
    (void)imsi;
    (void)meter_serial_number;
    (void)meter_type;

    cout << "[" << m_chargepoint->identifier() << "] - Boot notification : vendor = " << vendor << " - model = " << model
         << " - s/n = " << serial_number << " - firmware = " << firmware_version << endl;

    ocpp::types::RegistrationStatus ret = RegistrationStatus::Accepted;
    if (m_event_handler.setPendingEnabled())
    {
        if (!m_event_handler.isAcceptedChargePoint(m_chargepoint->identifier()))
        {
            m_event_handler.addPendingChargePoint(m_chargepoint);
            ret = RegistrationStatus::Pending;
        }
    }

    return ret;
}

/** @copydoc ocpp::types::DataTransferStatus IChargePointRequestHandler::dataTransfer(const std::string&,
                                                                                      const std::string&,
                                                                                      const std::string&,
                                                                                      std::string&) */
ocpp::types::DataTransferStatus DefaultCentralSystemEventsHandler::ChargePointRequestHandler::dataTransfer(const std::string& vendor_id,
                                                                                                           const std::string& message_id,
                                                                                                           const std::string& request_data,
                                                                                                           std::string&       response_data)
{
    cout << "[" << m_chargepoint->identifier() << "] - Data transfer : vendor = " << vendor_id << " - message = " << message_id
         << " - data = " << request_data << endl;

    response_data = "";
    return DataTransferStatus::UnknownVendorId;
}

/** @copydoc void IChargePointRequestHandler::diagnosticStatusNotification(ocpp::types::DiagnosticsStatus) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::diagnosticStatusNotification(ocpp::types::DiagnosticsStatus status)
{
    cout << "[" << m_chargepoint->identifier() << "] - Diagnostic status notification : " << DiagnosticsStatusHelper.toString(status)
         << endl;
}

/** @copydoc void IChargePointRequestHandler::firmwareStatusNotification(ocpp::types::FirmwareStatus) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::firmwareStatusNotification(ocpp::types::FirmwareStatus status)
{
    cout << "[" << m_chargepoint->identifier() << "] - Firmware status notification : " << FirmwareStatusHelper.toString(status) << endl;
}

/** @copydoc void IChargePointRequestHandler::meterValues(unsigned int,
                                                          const ocpp::types::Optional<int>&,
                                                          const std::vector<ocpp::types::MeterValue>&) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::meterValues(unsigned int                                connector_id,
                                                                               const ocpp::types::Optional<int>&           transaction_id,
                                                                               const std::vector<ocpp::types::MeterValue>& meter_values)
{
    cout << "[" << m_chargepoint->identifier() << "] - Meter values : connector = " << connector_id
         << " - transaction = " << (transaction_id.isSet() ? std::to_string(transaction_id) : "not set") << endl;
    for (const MeterValue& meter_value : meter_values)
    {
        cout << " - timestamp : " << meter_value.timestamp.str() << ", sampled values : " << endl;
        for (const SampledValue& sampled_value : meter_value.sampledValue)
        {
            cout << "    - value = " << sampled_value.value;
            if (sampled_value.unit.isSet())
            {
                cout << ", unit = " << UnitOfMeasureHelper.toString(sampled_value.unit);
            }
            if (sampled_value.phase.isSet())
            {
                cout << ", phase = " << PhaseHelper.toString(sampled_value.phase);
            }
            if (sampled_value.measurand.isSet())
            {
                cout << ", measurand = " << MeasurandHelper.toString(sampled_value.measurand);
            }
            if (sampled_value.context.isSet())
            {
                cout << ", context = " << ReadingContextHelper.toString(sampled_value.context);
            }
            if (sampled_value.location.isSet())
            {
                cout << ", location = " << LocationHelper.toString(sampled_value.location);
            }
            if (sampled_value.format.isSet())
            {
                cout << ", format = " << ValueFormatHelper.toString(sampled_value.format);
            }
            cout << endl;
        }
    }
}

/** @copydoc ocpp::types::IdTagInfo IChargePointRequestHandler::startTransaction(unsigned int,
                                                                                 const std::string&,
                                                                                 int,
                                                                                 const ocpp::types::Optional<int>&,
                                                                                 const ocpp::types::DateTime&,
                                                                                 int&) */
ocpp::types::IdTagInfo DefaultCentralSystemEventsHandler::ChargePointRequestHandler::startTransaction(
    unsigned int                      connector_id,
    const std::string&                id_tag,
    int                               meter_start,
    const ocpp::types::Optional<int>& reservation_id,
    const ocpp::types::DateTime&      timestamp,
    int&                              transaction_id)
{
    static int current_transaction_id = 1;

    cout << "[" << m_chargepoint->identifier() << "] - Start transaction : connector = " << connector_id << " - id tag = " << id_tag
         << " - meter start = " << meter_start
         << " - reservation = " << (reservation_id.isSet() ? std::to_string(reservation_id) : "not set")
         << " - timestamp = " << timestamp.str() << endl;

    transaction_id = current_transaction_id;
    current_transaction_id++;

    IdTagInfo tag_info;
    tag_info.status     = AuthorizationStatus::Accepted;
    tag_info.expiryDate = DateTime(DateTime::now().timestamp() + 3600);
    return tag_info;
}

/** @copydoc void IChargePointRequestHandler::statusNotification(unsigned int,
                                                                 ocpp::types::ChargePointErrorCode,
                                                                 const std::string&,
                                                                 ocpp::types::ChargePointStatus,
                                                                 const ocpp::types::DateTime&,
                                                                 const std::string&,
                                                                 const std::string&) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::statusNotification(unsigned int                      connector_id,
                                                                                      ocpp::types::ChargePointErrorCode error_code,
                                                                                      const std::string&                info,
                                                                                      ocpp::types::ChargePointStatus    status,
                                                                                      const ocpp::types::DateTime&      timestamp,
                                                                                      const std::string&                vendor_id,
                                                                                      const std::string&                vendor_error)
{
    (void)vendor_id;
    (void)vendor_error;

    cout << "[" << m_chargepoint->identifier() << "] - Status notification : connector = " << connector_id
         << " - status = " << ChargePointStatusHelper.toString(status) << " - error = " << ChargePointErrorCodeHelper.toString(error_code)
         << " - info = " << info << " - timestamp = " << ((timestamp == DateTime(0)) ? "not set" : timestamp.str()) << endl;
}

/** @copydoc ocpp::types::Optional<ocpp::types::IdTagInfo> IChargePointRequestHandler::stopTransaction(
                                                                                const std::string&,
                                                                                int,
                                                                                const ocpp::types::DateTime&,
                                                                                int,
                                                                                ocpp::types::Reason,
                                                                                const std::vector<ocpp::types::MeterValue>&) */
ocpp::types::Optional<ocpp::types::IdTagInfo> DefaultCentralSystemEventsHandler::ChargePointRequestHandler::stopTransaction(
    const std::string&                          id_tag,
    int                                         meter_stop,
    const ocpp::types::DateTime&                timestamp,
    int                                         transaction_id,
    ocpp::types::Reason                         reason,
    const std::vector<ocpp::types::MeterValue>& transaction_data)
{
    (void)transaction_data;
    cout << "[" << m_chargepoint->identifier() << "] - Stop transaction : transaction = " << transaction_id
         << " - id tag = " << (id_tag.empty() ? "not set" : id_tag) << " - meter stop = " << meter_stop
         << " - reason = " << ReasonHelper.toString(reason) << " - timestamp = " << timestamp.str() << endl;

    ocpp::types::Optional<ocpp::types::IdTagInfo> ret;
    if (!id_tag.empty())
    {
        IdTagInfo& tag_info = ret.value();
        tag_info.status     = AuthorizationStatus::Accepted;
        tag_info.expiryDate = DateTime(DateTime::now().timestamp() + 3600);
    }
    return ret;
}

// Security extensions

/** @copydoc void IChargePointRequestHandler::logStatusNotification(ocpp::types::UploadLogStatusEnumType, const ocpp::types::Optional<int>&) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::logStatusNotification(ocpp::types::UploadLogStatusEnumType status,
                                                                                         const ocpp::types::Optional<int>&    request_id)
{
    cout << "[" << m_chargepoint->identifier()
         << "] - Log status notification : status = " << UploadLogStatusEnumTypeHelper.toString(status)
         << " - request_id = " << (request_id.isSet() ? std::to_string(request_id) : "not set") << endl;
}

/** @copydoc void IChargePointRequestHandler::securityEventNotification(const std::string&,
                                                                        const ocpp::types::DateTime&,
                                                                        const std::string&) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::securityEventNotification(const std::string&           type,
                                                                                             const ocpp::types::DateTime& timestamp,
                                                                                             const std::string&           message)
{
    cout << "[" << m_chargepoint->identifier() << "] - Security event : timestamp = " << timestamp.str() << " - type = " << type
         << " - message = " << message << endl;
}

/** @copydoc bool IChargePointRequestHandler::signCertificate(const ocpp::x509::CertificateRequest&) */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::signCertificate(
    const ocpp::x509::CertificateRequest& certificate_request)
{
    bool ret = false;
    cout << "[" << m_chargepoint->identifier() << "] - Sign certificate : subject = " << certificate_request.subjectString() << endl;

    // Load CA certificate which will sign the request
    std::filesystem::path ca_cert_path(m_chargepoint->centralSystem().getConfig().tlsServerCertificateCa());
    Certificate           ca_cert(ca_cert_path);
    if (ca_cert.isValid())
    {
        const X509Document::Subject& ca_subject = ca_cert.subject();

        // Check CPO name and serial number
        const X509Document::Subject& subject = certificate_request.subject();
        if ((subject.organization == ca_subject.organization) &&
            (subject.common_name == getChargePointSerialNumber(m_chargepoint->identifier())))
        {
            // Save the request to a temporary file
            Sha2 sha256;
            sha256.compute(certificate_request.pem().c_str(), certificate_request.pem().size());

            std::stringstream name;
            name << "/tmp/csr_" << sha256.resultString() << ".pem";
            std::string csr_filename = name.str();
            if (certificate_request.toFile(csr_filename))
            {
                // Sign the certificate request to generate a certificate
                std::string ca_cert_key_path = ca_cert_path.string();
                ocpp::helpers::replace(ca_cert_key_path, ".pem", ".key");
                ocpp::helpers::replace(ca_cert_key_path, ".crt", ".key");
                std::string       certificate_filename = csr_filename + ".crt";
                std::stringstream sign_cert_cmd_line;
                sign_cert_cmd_line << "openssl x509 -req -sha256 -days 3650 -in " << csr_filename << " -CA " << ca_cert_path << " -CAkey "
                                   << ca_cert_key_path << " -CAcreateserial -out " << certificate_filename;
                int err = WEXITSTATUS(system(sign_cert_cmd_line.str().c_str()));
                cout << "Command line : " << sign_cert_cmd_line.str() << " => " << err << endl;

                // Check if the certificate has been generated
                if (std::filesystem::exists(certificate_filename))
                {
                    // Create bundle
                    std::string       bundle_filename = certificate_filename + ".bundle";
                    std::stringstream generate_bundle_cmd_line;
                    generate_bundle_cmd_line << "cat " << certificate_filename << " " << ca_cert_path << " > " << bundle_filename;
                    err = WEXITSTATUS(system(generate_bundle_cmd_line.str().c_str()));
                    cout << "Command line : " << generate_bundle_cmd_line.str() << " => " << err << endl;
                    if (std::filesystem::exists(bundle_filename))
                    {
                        m_generated_certificate = bundle_filename;
                        ret                     = true;
                    }
                    else
                    {
                        cout << "[" << m_chargepoint->identifier()
                             << "] - Failed to generate certificate bundle : " << generate_bundle_cmd_line.str() << endl;
                    }
                }
                else
                {
                    cout << "[" << m_chargepoint->identifier() << "] - Failed to sign the CSR : " << sign_cert_cmd_line.str() << endl;
                }

                // Remove the temporary files
                std::filesystem::remove(csr_filename);
                std::filesystem::remove(certificate_filename);
            }
            else
            {
                cout << "[" << m_chargepoint->identifier() << "] - Unable to create CSR file : " << csr_filename << endl;
            }
        }
        else
        {
            cout << "[" << m_chargepoint->identifier() << "] - Invalid organization or common name" << endl;
        }
    }
    else
    {
        cout << "[" << m_chargepoint->identifier() << "] - Unable to load CA certificate : " << ca_cert_path << endl;
    }
    return ret;
}

/** @copydoc void IChargePointRequestHandler::signedFirmwareUpdateStatusNotification(ocpp::types::FirmwareStatusEnumType,
                                                                                             const ocpp::types::Optional<int>&) */
void DefaultCentralSystemEventsHandler::ChargePointRequestHandler::signedFirmwareUpdateStatusNotification(
    ocpp::types::FirmwareStatusEnumType status, const ocpp::types::Optional<int>& request_id)
{
    cout << "[" << m_chargepoint->identifier()
         << "] - Signed firmware update status notification : status = " << FirmwareStatusEnumTypeHelper.toString(status)
         << " - request_id = " << (request_id.isSet() ? std::to_string(request_id) : "not set") << endl;
}

// ISO 15118 PnC extensions

/** @copydoc ocpp::types::IdTokenInfoType IChargePointRequestHandler::iso15118Authorize(
                                                          const ocpp::x509::Certificate&,
                                                          const std::string&,
                                                          const std::vector<ocpp::types::OcspRequestDataType>&,
                                                          ocpp::types::Optional<ocpp::types::AuthorizeCertificateStatusEnumType>&) override; */
ocpp::types::IdTokenInfoType DefaultCentralSystemEventsHandler::ChargePointRequestHandler::iso15118Authorize(
    const ocpp::x509::Certificate&                                          certificate,
    const std::string&                                                      id_token,
    const std::vector<ocpp::types::OcspRequestDataType>&                    cert_hash_data,
    ocpp::types::Optional<ocpp::types::AuthorizeCertificateStatusEnumType>& cert_status)
{
    cout << "[" << m_chargepoint->identifier() << "] - [ISO15118] Authorize : certificate = " << certificate.pem().size()
         << " - id_token = " << id_token << " - cert_hash_data size = " << cert_hash_data.size() << endl;

    // Prepare response
    ocpp::types::IdTokenInfoType ret;
    ret.status = AuthorizationStatus::Invalid;

    // Check certificate if present
    if (certificate.isValid())
    {
        cert_status = AuthorizeCertificateStatusEnumType::Accepted;
        ret.status  = AuthorizationStatus::Accepted;
    }
    else
    {
        if (!cert_hash_data.empty())
        {
            // Forward to OCSP => TODO with OpenSSL
        }

        // For now, always accept
        ret.status = AuthorizationStatus::Accepted;
    }

    return ret;
}

/** @copydoc ocpp::types::Iso15118EVCertificateStatusEnumType IChargePointRequestHandler::iso15118GetEVCertificate(
                                                          const std::string&,
                                                          ocpp::types::CertificateActionEnumType,
                                                          const std::string&,
                                                          std::string&) */
ocpp::types::Iso15118EVCertificateStatusEnumType DefaultCentralSystemEventsHandler::ChargePointRequestHandler::iso15118GetEVCertificate(
    const std::string&                     iso15118_schema_version,
    ocpp::types::CertificateActionEnumType action,
    const std::string&                     exi_request,
    std::string&                           exi_response)
{
    cout << "[" << m_chargepoint->identifier()
         << "] - [ISO15118] Get EV certificate : iso15118_schema_version = " << iso15118_schema_version
         << " - action = " << CertificateActionEnumTypeHelper.toString(action) << " - exi_request size = " << exi_request.size() << endl;

    // For the purpose of this example, the EXI response contains directly the EV certificate in PEM format
    // In a real system, the certificate is embedded in an EXI message

    // Generate CSR for the EV certificate
    X509Document::Subject ev_cert_subject;
    ev_cert_subject.country           = "FR";
    ev_cert_subject.state             = "Savoie";
    ev_cert_subject.location          = "Chambery";
    ev_cert_subject.organization      = "Open OCPP";
    ev_cert_subject.organization_unit = "Examples";
    ev_cert_subject.common_name       = "MO EV certificate";
    ev_cert_subject.email_address     = "ca.examples@open-ocpp.org";
    PrivateKey         ev_cert_key(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "");
    CertificateRequest ev_cert_req(ev_cert_subject, ev_cert_key, Sha2::SHA256);

    // Sign the certificate with the MO root certificate
    Certificate mo_root_ca(m_event_handler.moRootCA());
    std::string mo_root_ca_key_path = m_event_handler.moRootCA().string();
    ocpp::helpers::replace(mo_root_ca_key_path, ".pem", ".key");
    PrivateKey  mo_root_ca_key(std::filesystem::path(mo_root_ca_key_path), "");
    Certificate ev_cert(ev_cert_req, mo_root_ca, mo_root_ca_key, Sha2::SHA256, 7300);

    // Put certificate inside the response
    exi_response = ev_cert.pem();

    return Iso15118EVCertificateStatusEnumType::Accepted;
}

/** @copydoc ocpp::types::GetCertificateStatusEnumType IChargePointRequestHandler::iso15118GetCertificateStatus(
                                                          const ocpp::types::OcspRequestDataType&,
                                                          std::string&) */
ocpp::types::GetCertificateStatusEnumType DefaultCentralSystemEventsHandler::ChargePointRequestHandler::iso15118GetCertificateStatus(
    const ocpp::types::OcspRequestDataType& ocsp_request, std::string& ocsp_result)
{

    cout << "[" << m_chargepoint->identifier()
         << "] - [ISO15118] Get EV certificate status : serial number = " << ocsp_request.serialNumber.str() << endl;

    // Forward to OCSP => TODO with OpenSSL
    (void)ocsp_result;

    return GetCertificateStatusEnumType::Accepted;
}

/** @copydoc bool iso15118SignCertificate(const ocpp::x509::CertificateRequest&) */
bool DefaultCentralSystemEventsHandler::ChargePointRequestHandler::iso15118SignCertificate(
    const ocpp::x509::CertificateRequest& certificate_request)
{
    bool ret = false;
    cout << "[" << m_chargepoint->identifier() << "] - [ISO15118] Sign certificate : subject = " << certificate_request.subjectString()
         << endl;

    // Load CA certificate which will sign the request
    std::filesystem::path ca_cert_path = m_event_handler.v2gRootCA();
    Certificate           ca_cert(ca_cert_path);
    if (ca_cert.isValid())
    {
        // Save the request to a temporary file
        Sha2 sha256;
        sha256.compute(certificate_request.pem().c_str(), certificate_request.pem().size());

        std::stringstream name;
        name << "/tmp/csr_" << sha256.resultString() << ".pem";
        std::string csr_filename = name.str();
        if (certificate_request.toFile(csr_filename))
        {
            // Sign the certificate request to generate a certificate
            std::string ca_cert_key_path = ca_cert_path.string();
            ocpp::helpers::replace(ca_cert_key_path, ".pem", ".key");
            ocpp::helpers::replace(ca_cert_key_path, ".crt", ".key");
            std::string       certificate_filename = csr_filename + ".crt";
            std::stringstream sign_cert_cmd_line;
            sign_cert_cmd_line << "openssl x509 -req -sha256 -days 3650 -in " << csr_filename << " -CA " << ca_cert_path << " -CAkey "
                               << ca_cert_key_path << " -CAcreateserial -out " << certificate_filename;
            int err = WEXITSTATUS(system(sign_cert_cmd_line.str().c_str()));
            cout << "Command line : " << sign_cert_cmd_line.str() << " => " << err << endl;

            // Check if the certificate has been generated
            if (std::filesystem::exists(certificate_filename))
            {
                // Create bundle
                std::string       bundle_filename = certificate_filename + ".bundle";
                std::stringstream generate_bundle_cmd_line;
                generate_bundle_cmd_line << "cat " << certificate_filename << " " << ca_cert_path << " > " << bundle_filename;
                err = WEXITSTATUS(system(generate_bundle_cmd_line.str().c_str()));
                cout << "Command line : " << generate_bundle_cmd_line.str() << " => " << err << endl;
                if (std::filesystem::exists(bundle_filename))
                {
                    m_generated_certificate = bundle_filename;
                    ret                     = true;
                }
                else
                {
                    cout << "[" << m_chargepoint->identifier()
                         << "] - [ISO15118] Failed to generate certificate bundle : " << generate_bundle_cmd_line.str() << endl;
                }
            }
            else
            {
                cout << "[" << m_chargepoint->identifier() << "] - [ISO15118] Failed to sign the CSR : " << sign_cert_cmd_line.str()
                     << endl;
            }

            // Remove the temporary files
            std::filesystem::remove(csr_filename);
            std::filesystem::remove(certificate_filename);
        }
        else
        {
            cout << "[" << m_chargepoint->identifier() << "] - [ISO15118] Unable to create CSR file : " << csr_filename << endl;
        }
    }
    else
    {
        cout << "[" << m_chargepoint->identifier() << "] - [ISO15118] Unable to load CA certificate : " << ca_cert_path << endl;
    }
    return ret;
}