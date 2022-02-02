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

#include <iomanip>
#include <sstream>

#include <openssl/bio.h>
#include <openssl/pem.h>
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

/** @brief Destructor */
Certificate::~Certificate() { }

/** @brief Verify the PEM certificate chain */
bool Certificate::verify() const
{
    bool ret = false;

    // Check if it is a certificate chain
    if (m_certificate_chain.size() > 1u)
    {
        // Create verify store context
        X509_STORE_CTX* store_context = X509_STORE_CTX_new();

        // Load certificate to check
        X509* cert = reinterpret_cast<X509*>(loadX509(m_pem_chain[0]));
        if (cert)
        {
            X509_STORE_CTX_init(store_context, nullptr, cert, nullptr);

            // Create sub-CA certificates stack = not self-signed certificates
            STACK_OF(X509)* sub_cas = sk_X509_new_null();
            std::vector<X509*> sub_cas_vec;
            for (size_t i = 1u; i < m_certificate_chain.size(); i++)
            {
                const Certificate& c = m_certificate_chain[i];
                if (!c.isSelfSigned())
                {
                    X509* x509_cert = reinterpret_cast<X509*>(loadX509(c.pem()));
                    if (x509_cert)
                    {
                        sk_X509_push(sub_cas, x509_cert);
                        sub_cas_vec.push_back(x509_cert);
                    }
                }
            }
            X509_STORE_CTX_set0_untrusted(store_context, sub_cas);

            // Create CA certificates stack = self-signed certificates
            STACK_OF(X509)* cas = sk_X509_new_null();
            std::vector<X509*> cas_vec;
            for (size_t i = 1u; i < m_certificate_chain.size(); i++)
            {
                const Certificate& c = m_certificate_chain[i];
                if (c.isSelfSigned())
                {
                    X509* x509_cert = reinterpret_cast<X509*>(loadX509(c.pem()));
                    if (x509_cert)
                    {
                        sk_X509_push(cas, x509_cert);
                        cas_vec.push_back(x509_cert);
                    }
                }
            }
            X509_STORE_CTX_set0_trusted_stack(store_context, cas);

            // Verifiy certificate chain
            if (X509_verify_cert(store_context) == 1)
            {
                ret = true;
            }

            // Release memory
            sk_X509_free(cas);
            for (X509* x509_cert : cas_vec)
            {
                X509_free(x509_cert);
            }
            sk_X509_free(sub_cas);
            for (X509* x509_cert : sub_cas_vec)
            {
                X509_free(x509_cert);
            }
            X509_free(cert);
        }

        // Release verify context
        X509_STORE_CTX_free(store_context);
    }

    return ret;
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
            certificate.m_x509v3_extensions.emplace_back(OBJ_nid2ln(extension_obj_nid));
            if (extension_obj_nid == NID_issuer_alt_name)
            {
                certificate.m_issuer_alternate_names = convertGeneralNames(X509_get_ext_d2i(cert, NID_issuer_alt_name, NULL, NULL));
            }
            else if (extension_obj_nid == NID_subject_alt_name)
            {
                certificate.m_subject_alternate_names = convertGeneralNames(X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL));
            }
            else
            {
            }
        }

        X509_free(cert);
    }
}

} // namespace x509
} // namespace ocpp
