/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Certificate.h"
#include "CertificateRequest.h"
#include "PrivateKey.h"
#include "sign.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

namespace ocpp
{
namespace x509
{

/** @brief Constructor from PEM file */
Certificate::Certificate(const std::filesystem::path& pem_file) : X509Document(pem_file)
{
    // Extract PEM chain
    extractPemChain();
}

/** @brief Constructor from PEM data */
Certificate::Certificate(const std::string& pem_data) : X509Document(pem_data)
{
    // Extract PEM chain
    extractPemChain();
}

/** @brief Constructor from certificate request and signing certificate */
Certificate::Certificate(const CertificateRequest& certificate_request,
                         const Certificate&        signing_certificate,
                         const PrivateKey&         private_key,
                         Sha2::Type                sha,
                         unsigned int              days)
    : X509Document(std::string(""))
{
    // Convert request to certificate
    X509_REQ*   req          = const_cast<X509_REQ*>(reinterpret_cast<const X509_REQ*>(certificate_request.object()));
    EVP_PKEY*   pkey         = const_cast<EVP_PKEY*>(reinterpret_cast<const EVP_PKEY*>(private_key.object()));
    const X509* signing_cert = reinterpret_cast<const X509*>(signing_certificate.object());
    convertCertificateRequest(req, signing_cert, pkey, sha, days);
}

/** @brief Constructor for a self-signed certificate from a certificate request */
Certificate::Certificate(const CertificateRequest& certificate_request, const PrivateKey& private_key, Sha2::Type sha, unsigned int days)
    : X509Document(std::string(""))
{
    // Convert request to certificate
    X509_REQ* req  = const_cast<X509_REQ*>(reinterpret_cast<const X509_REQ*>(certificate_request.object()));
    EVP_PKEY* pkey = const_cast<EVP_PKEY*>(reinterpret_cast<const EVP_PKEY*>(private_key.object()));
    convertCertificateRequest(req, nullptr, pkey, sha, days);
}

/** @brief Copy constructor */
Certificate::Certificate(const Certificate& copy)
    : X509Document(copy),
      m_pem_chain(copy.m_pem_chain),
      m_certificate_chain(copy.m_certificate_chain),
      m_serial_number(copy.m_serial_number),
      m_serial_number_string(copy.m_serial_number_string),
      m_serial_number_hex_string(copy.m_serial_number_hex_string),
      m_validity_from(copy.m_validity_from),
      m_validity_to(copy.m_validity_to),
      m_issuer(copy.m_issuer),
      m_issuer_string(copy.m_issuer_string),
      m_is_self_signed(copy.m_is_self_signed)
{
    // Duplicate OpenSSL object
    if (copy.m_openssl_object)
    {
        m_openssl_object = X509_dup(reinterpret_cast<X509*>(copy.m_openssl_object));
    }
}

/** @brief Destructor */
Certificate::~Certificate()
{
    X509_free(reinterpret_cast<X509*>(m_openssl_object));
}

/** @brief Verify the PEM certificate chain */
bool Certificate::verify() const
{
    bool ret = false;

    // Check if it is a certificate chain
    if (m_certificate_chain.size() > 1u)
    {
        ret = verify(*this, m_certificate_chain, 1u);
    }

    return ret;
}

/** @brief Verify the certificate signature against a certificate chain */
bool Certificate::verify(const std::vector<Certificate>& ca_chain) const
{
    bool ret = false;

    // Check if the certificate is valid
    if (m_is_valid)
    {
        ret = verify(*this, ca_chain, 0u);
    }

    return ret;
}

/** @brief Verify the signature of a buffer using the certificate's public key */
bool Certificate::verify(const std::vector<uint8_t>& signature, const void* buffer, size_t size, Sha2::Type sha)
{
    X509*     cert = reinterpret_cast<X509*>(m_openssl_object);
    EVP_PKEY* pkey = X509_get0_pubkey(cert);
    return ocpp::x509::verify(signature, buffer, size, sha, pkey);
}

/** @brief Verify the signature of a file using the certificate's public key */
bool Certificate::verify(const std::vector<uint8_t>& signature, const std::string& filepath, Sha2::Type sha)
{
    X509*     cert = reinterpret_cast<X509*>(m_openssl_object);
    EVP_PKEY* pkey = X509_get0_pubkey(cert);
    return ocpp::x509::verify(signature, filepath, sha, pkey);
}

/** @brief Extract all the PEM certificates in the certificate chain */
void Certificate::extractPemChain()
{
    std::string pem = m_pem;

    // Look for multiple header/footers
    size_t pos_start = 0;
    size_t pos_end   = 0;
    do
    {
        size_t begin = std::string::npos;
        size_t end   = std::string::npos;

        pos_start = pem.find("-----", pos_start);
        if (pos_start != std::string::npos)
        {
            pos_end = pem.find("-----", pos_start + 5u);
            if (pos_end != std::string::npos)
            {
                begin = pos_start;
            }
        }
        pos_start = pem.find("-----", pos_end + 5u);
        if (pos_start != std::string::npos)
        {
            pos_end = pem.find("-----", pos_start + 5u);
            if (pos_end != std::string::npos)
            {
                end       = pos_end + 5u;
                pos_start = end;
            }
        }
        if ((begin != std::string::npos) && (end != std::string::npos))
        {
            // Save PEM
            m_pem_chain.emplace_back(pem.substr(begin, end - begin));
        }

    } while ((pos_start != std::string::npos) && (pos_end != std::string::npos));
    if (m_pem_chain.size() > 0)
    {
        // Primary certificate
        readInfos(*this);

        // Read the certificate chain
        if (m_pem_chain.size() > 1)
        {
            for (size_t i = 0; i < m_pem_chain.size(); i++)
            {
                m_certificate_chain.emplace_back(m_pem_chain[i]);
            }
        }
        else
        {
            m_certificate_chain.emplace_back(*this);
        }
    }
}

/** @brief Converts a certificate request to a certificate */
void Certificate::convertCertificateRequest(void* request, const void* issuer, void* key, Sha2::Type sha, unsigned int days)
{
    X509_REQ*   cert_request = reinterpret_cast<X509_REQ*>(request);
    const X509* issuer_cert  = reinterpret_cast<const X509*>(issuer);
    EVP_PKEY*   private_key  = reinterpret_cast<EVP_PKEY*>(key);

    X509* cert = X509_new();

    // Set version
    X509_set_version(cert, X509_REQ_get_version(cert_request));

    // Set validity dates
    ASN1_TIME* validity = ASN1_TIME_new();
    ASN1_TIME_set(validity, time(nullptr));
    X509_set1_notBefore(cert, validity);
    ASN1_TIME_set(validity, time(nullptr) + (days * 24 * 3600));
    X509_set1_notAfter(cert, validity);
    ASN1_TIME_free(validity);

    // Set serial number
    uint8_t serial_bytes[20];
    RAND_bytes(serial_bytes, sizeof(serial_bytes));
    ASN1_INTEGER* serial = ASN1_INTEGER_new();
    ASN1_STRING_set(serial, serial_bytes, sizeof(serial_bytes));
    X509_set_serialNumber(cert, serial);

    // Set subject and issuer name
    X509_NAME* issuer_name;
    if (issuer_cert)
    {
        issuer_name = X509_get_subject_name(issuer_cert);
    }
    else
    {
        issuer_name = X509_REQ_get_subject_name(cert_request);
    }
    X509_set_issuer_name(cert, issuer_name);
    X509_set_subject_name(cert, X509_REQ_get_subject_name(cert_request));

    // Set the public key
    EVP_PKEY* public_key = X509_REQ_get_pubkey(cert_request);
    X509_set_pubkey(cert, public_key);

    // Set the extensions
    STACK_OF(X509_EXTENSION)* extensions = X509_REQ_get_extensions(cert_request);
    if (extensions)
    {
        int extensions_count = X509v3_get_ext_count(extensions);
        for (int i = 0; i < extensions_count; i++)
        {
            X509_EXTENSION* extension = X509v3_get_ext(extensions, i);
            X509_add_ext(cert, extension, -1);
            if (!issuer_cert)
            {
                if (OBJ_obj2nid(X509_EXTENSION_get_object(extension)) == NID_subject_alt_name)
                {
                    X509_EXTENSION_set_object(extension, OBJ_nid2obj(NID_issuer_alt_name));
                    X509_add_ext(cert, extension, -1);
                }
            }
        }
        sk_X509_EXTENSION_free(extensions);
    }
    if (issuer_cert)
    {
        int   crit = 0;
        int   idx  = 0;
        void* val  = X509_get_ext_d2i(issuer_cert, NID_subject_alt_name, &crit, &idx);
        if (val)
        {
            X509_add1_ext_i2d(cert, NID_issuer_alt_name, val, crit, 0);
            OPENSSL_free(val);
        }
    }

    // Sign the certificate
    const EVP_MD* digest = nullptr;
    if (sha == Sha2::Type::SHA256)
    {
        digest = EVP_sha256();
    }
    else if (sha == Sha2::Type::SHA384)
    {
        digest = EVP_sha384();
    }
    else
    {
        digest = EVP_sha512();
    }
    X509_sign(cert, private_key, digest);

    // Convert to PEM
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_X509(bio, cert);
    char* bio_data = nullptr;
    int   bio_len  = BIO_get_mem_data(bio, &bio_data);
    m_pem.insert(0, bio_data, static_cast<size_t>(bio_len));
    BIO_free(bio);

    // Release memory
    X509_free(cert);

    // Read PEM infos
    extractPemChain();
}

/** @brief Load OpenSSL X509 certificate structure from a PEM encoded data string */
void* Certificate::loadX509(const std::string& pem_data)
{
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, pem_data.c_str(), static_cast<int>(pem_data.size()));
    void* cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    BIO_free(bio);
    return cert;
}

/** @brief Read X509 informations stored inside a certificate */
void Certificate::readInfos(Certificate& certificate)
{
    // Load PEM
    X509* cert = reinterpret_cast<X509*>(loadX509(certificate.m_pem));
    if (cert)
    {
        // Certificate is valid
        certificate.m_is_valid = true;

        // Extract serial number
        const ASN1_INTEGER*  serial_number_asn1 = X509_get0_serialNumber(cert);
        int                  serial_len         = ASN1_STRING_length(serial_number_asn1);
        const unsigned char* serial             = ASN1_STRING_get0_data(serial_number_asn1);
        std::stringstream    ss_serial;
        std::stringstream    ss_serial_hex;
        ss_serial << std::hex;
        ss_serial_hex << std::hex;
        for (int i = 0; i < serial_len; i++)
        {
            ss_serial << std::setw(2) << std::setfill('0') << static_cast<int>(serial[i]) << ":";
            ss_serial_hex << std::setw(2) << std::setfill('0') << static_cast<int>(serial[i]);
            certificate.m_serial_number.push_back(serial[i]);
        }
        certificate.m_serial_number_string = ss_serial.str();
        certificate.m_serial_number_string.resize(certificate.m_serial_number_string.size() - 1u);
        certificate.m_serial_number_hex_string = ss_serial_hex.str();

        // Extract validity dates
        certificate.m_validity_from = convertAsn1Time(X509_get0_notBefore(cert));
        certificate.m_validity_to   = convertAsn1Time(X509_get0_notAfter(cert));

        // Extract issuer and subject
        X509_NAME* issuer           = X509_get_issuer_name(cert);
        certificate.m_issuer_string = convertX509Name(issuer);
        parseSubjectString(issuer, certificate.m_issuer);
        X509_NAME* subject           = X509_get_subject_name(cert);
        certificate.m_subject_string = convertX509Name(subject);
        parseSubjectString(subject, certificate.m_subject);
        certificate.m_is_self_signed = (certificate.m_issuer_string == certificate.m_subject_string);

        // Extract signature algorithm name
        int sig_nid = 0;
        int pk_nid  = 0;
        X509_get_signature_info(cert, &sig_nid, &pk_nid, nullptr, nullptr);
        certificate.m_sig_hash = OBJ_nid2sn(sig_nid);
        certificate.m_sig_algo = OBJ_nid2sn(X509_get_signature_nid(cert));

        // Extract public key infos
        EVP_PKEY* pub_key_cert = X509_get0_pubkey(cert);
        certificate.parsePublicKey(pub_key_cert);

        // Extract extensions
        const STACK_OF(X509_EXTENSION)* extensions = X509_get0_extensions(cert);
        int extensions_count                       = X509v3_get_ext_count(extensions);
        for (int i = 0; i < extensions_count; i++)
        {
            X509_EXTENSION* extension         = X509v3_get_ext(extensions, i);
            ASN1_OBJECT*    extension_obj     = X509_EXTENSION_get_object(extension);
            int             extension_obj_nid = OBJ_obj2nid(extension_obj);
            certificate.m_x509v3_extensions_names.emplace_back(OBJ_nid2ln(extension_obj_nid));
            if (extension_obj_nid == NID_issuer_alt_name)
            {
                certificate.m_x509v3_extensions.issuer_alternate_names =
                    convertGeneralNames(X509_get_ext_d2i(cert, NID_issuer_alt_name, nullptr, nullptr));
            }
            else if (extension_obj_nid == NID_subject_alt_name)
            {
                certificate.m_x509v3_extensions.subject_alternate_names =
                    convertGeneralNames(X509_get_ext_d2i(cert, NID_subject_alt_name, nullptr, nullptr));
            }
            else if (extension_obj_nid == NID_basic_constraints)
            {
                BASIC_CONSTRAINTS* basic_constraint = (BASIC_CONSTRAINTS*)X509_get_ext_d2i(cert, NID_basic_constraints, nullptr, nullptr);
                if (basic_constraint)
                {
                    certificate.m_x509v3_extensions.basic_constraints.present = true;
                    if (basic_constraint->ca != 0)
                    {
                        certificate.m_x509v3_extensions.basic_constraints.is_ca = true;
                        if (basic_constraint->pathlen)
                        {
                            certificate.m_x509v3_extensions.basic_constraints.path_length = ASN1_INTEGER_get(basic_constraint->pathlen);
                        }
                    }
                }
            }
            else
            {
            }
        }

        // Save OpenSSL object
        certificate.m_openssl_object = cert;
    }
}

/** @brief Verify a certificate against a chain of certificates */
bool Certificate::verify(const Certificate& certificate, const std::vector<Certificate>& certificate_chain, size_t start_index)
{
    bool ret = false;

    // Create verify store context
    X509_STORE_CTX* store_context = X509_STORE_CTX_new();

    // Load certificate to check
    X509_STORE_CTX_init(store_context, nullptr, reinterpret_cast<X509*>(certificate.m_openssl_object), nullptr);

    // Create sub-CA certificates stack = not self-signed certificates
    STACK_OF(X509)* sub_cas = sk_X509_new_null();
    for (size_t i = start_index; i < certificate_chain.size(); i++)
    {
        const Certificate& c = certificate_chain[i];
        if (!c.isSelfSigned())
        {
            X509* x509_cert = reinterpret_cast<X509*>(c.m_openssl_object);
            if (x509_cert)
            {
                sk_X509_push(sub_cas, x509_cert);
            }
        }
    }
    X509_STORE_CTX_set0_untrusted(store_context, sub_cas);

    // Create CA certificates stack = self-signed certificates
    STACK_OF(X509)* cas = sk_X509_new_null();
    for (size_t i = start_index; i < certificate_chain.size(); i++)
    {
        const Certificate& c = certificate_chain[i];
        if (c.isSelfSigned())
        {
            X509* x509_cert = reinterpret_cast<X509*>(c.m_openssl_object);
            if (x509_cert)
            {
                sk_X509_push(cas, x509_cert);
            }
        }
    }
    X509_STORE_CTX_set0_trusted_stack(store_context, cas);

    // Verify certificate chain
    if (X509_verify_cert(store_context) == 1)
    {
        ret = true;
    }

    // Release memory
    sk_X509_free(cas);
    sk_X509_free(sub_cas);

    // Release verify context
    X509_STORE_CTX_free(store_context);

    return ret;
}
} // namespace x509
} // namespace ocpp
