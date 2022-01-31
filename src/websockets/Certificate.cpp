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
#include "String.h"

#include <fstream>
#include <iomanip>
#include <sstream>

#include <arpa/inet.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

namespace ocpp
{
namespace websockets
{

/** @brief Constructor from PEM file */
Certificate::Certificate(const std::filesystem::path& pem_file) : m_is_valid(false), m_pem()
{
    // Open PEM file
    std::fstream file(pem_file, file.in | file.binary | file.ate);
    if (file.is_open())
    {
        // Read the whole file
        auto filesize = file.tellg();
        file.seekg(0, file.beg);
        m_pem.resize(filesize);
        file.read(&m_pem[0], filesize);

        // Extract PEM chain
        extractPemChain();
    }
}

/** @brief Constructor from PEM data */
Certificate::Certificate(const std::string& pem_data) : m_is_valid(false), m_pem(pem_data)
{
    // Extract PEM chain
    extractPemChain();
}

/** @brief Destructor */
Certificate::~Certificate() { }

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

/** @brief Read X509 informations stored inside a certificate */
void Certificate::readInfos(Certificate& certificate)
{
    // Load PEM
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, certificate.m_pem.c_str(), static_cast<int>(certificate.m_pem.size()));
    X509* cert = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (cert)
    {
        // Certificate is valid
        certificate.m_is_valid = true;

        // Extract serial number
        const ASN1_INTEGER*  serial_number_asn1 = X509_get0_serialNumber(cert);
        int                  serial_len         = ASN1_STRING_length(serial_number_asn1);
        const unsigned char* serial             = ASN1_STRING_get0_data(serial_number_asn1);
        std::stringstream    ss_serial;
        ss_serial << std::hex;
        for (int i = 0; i < serial_len; i++)
        {
            ss_serial << std::setw(2) << std::setfill('0') << static_cast<int>(serial[i]) << ":";
            certificate.m_serial_number.push_back(serial[i]);
        }
        certificate.m_serial_number_string = ss_serial.str();
        certificate.m_serial_number_string.resize(certificate.m_serial_number_string.size() - 1u);

        // Extract validity dates
        certificate.m_validity_from = convertAsn1Time(X509_get0_notBefore(cert));
        certificate.m_validity_to   = convertAsn1Time(X509_get0_notAfter(cert));

        // Extract issuer and subject
        certificate.m_issuer_string = convertX509Name(X509_get_issuer_name(cert));
        parseSubjectString(certificate.m_issuer_string, certificate.m_issuer);
        certificate.m_subject_string = convertX509Name(X509_get_subject_name(cert));
        parseSubjectString(certificate.m_subject_string, certificate.m_subject);

        // Extract signature algorithm name
        int sig_nid = 0;
        int pk_nid  = 0;
        X509_get_signature_info(cert, &sig_nid, &pk_nid, nullptr, nullptr);
        certificate.m_sig_hash = OBJ_nid2sn(sig_nid);
        certificate.m_sig_algo = OBJ_nid2sn(X509_get_signature_nid(cert));

        // Extract public key infos
        certificate.m_pub_key_algo = OBJ_nid2sn(pk_nid);

        EVP_PKEY*    pub_key_cert = X509_get0_pubkey(cert);
        X509_PUBKEY* pub_key      = X509_PUBKEY_new();
        X509_PUBKEY_set(&pub_key, pub_key_cert);

        ASN1_OBJECT*         pkalg = nullptr;
        int                  pklen = 0;
        const unsigned char* k     = nullptr;
        X509_ALGOR*          a;
        X509_PUBKEY_get0_param(&pkalg, &k, &pklen, &a, pub_key);
        if (a->parameter->type == V_ASN1_OBJECT)
        {
            int pub_key_algo_param_nid       = OBJ_obj2nid(a->parameter->value.object);
            certificate.m_pub_key_algo_param = OBJ_nid2sn(pub_key_algo_param_nid);
        }

        std::stringstream ss_pubkey;
        ss_pubkey << std::hex;
        for (int i = 0; i < pklen; i++)
        {
            ss_pubkey << std::setw(2) << std::setfill('0') << static_cast<int>(k[i]) << ":";
            certificate.m_pub_key.push_back(k[i]);
        }
        certificate.m_pub_key_string = ss_pubkey.str();
        certificate.m_pub_key_string.resize(certificate.m_pub_key_string.size() - 1u);

        X509_PUBKEY_free(pub_key);

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

/** @brief Convert a date in ASN1_TIME format to a standard time_t representation */
time_t Certificate::convertAsn1Time(const void* pasn1_time)
{
    const ASN1_TIME* asn1_time = reinterpret_cast<const ASN1_TIME*>(pasn1_time);
    struct tm        tm;
    ASN1_TIME_to_tm(asn1_time, &tm);
    return mktime(&tm);
}
/** @brief Convert a string in X509_NAME format to a standard string representation */
std::string Certificate::convertX509Name(const void* px509_name)
{
    std::string      name;
    const X509_NAME* x509_name = reinterpret_cast<const X509_NAME*>(px509_name);

    BIO* bio = BIO_new(BIO_s_mem());
    if (bio)
    {
        X509_NAME_print_ex(bio, x509_name, 0, XN_FLAG_ONELINE);
        char* bio_data = nullptr;
        int   bio_len  = BIO_get_mem_data(bio, &bio_data);
        name.insert(0, bio_data, static_cast<size_t>(bio_len));
        BIO_free(bio);
    }

    return name;
}

/** @brief Convert a list of strings in GENERAL_NAMES format to a standard vector of strings representation */
std::vector<std::string> Certificate::convertGeneralNames(const void* pgeneral_names)
{
    std::vector<std::string> names;
    const STACK_OF(GENERAL_NAME)* general_names = reinterpret_cast<const STACK_OF(GENERAL_NAME)*>(pgeneral_names);

    int names_count = sk_GENERAL_NAME_num(general_names);
    for (int i = 0; i < names_count; i++)
    {
        const GENERAL_NAME* current_name = sk_GENERAL_NAME_value(general_names, i);
        if (current_name->type == GEN_DNS)
        {
            char* name = nullptr;
            ASN1_STRING_to_UTF8((unsigned char**)&name, current_name->d.dNSName);
            names.emplace_back(name);
            OPENSSL_free(name);
        }
        else if (current_name->type == GEN_IPADD)
        {
            char* name = nullptr;
            int   len  = ASN1_STRING_length(current_name->d.iPAddress);
            if (len == 4)
            {
                char ip_address[INET_ADDRSTRLEN];
                inet_ntop(
                    AF_INET, ASN1_STRING_get0_data(current_name->d.iPAddress), ip_address, static_cast<socklen_t>(sizeof(ip_address)));
                names.emplace_back(ip_address);
            }
            else
            {
                char ip_address[INET6_ADDRSTRLEN];
                inet_ntop(
                    AF_INET6, ASN1_STRING_get0_data(current_name->d.iPAddress), ip_address, static_cast<socklen_t>(sizeof(ip_address)));
                names.emplace_back(ip_address);
            }
            OPENSSL_free(name);
        }
        else if (current_name->type == GEN_EMAIL)
        {
            char* name = nullptr;
            ASN1_STRING_to_UTF8((unsigned char**)&name, current_name->d.dNSName);
            names.emplace_back(name);
            OPENSSL_free(name);
        }
        else
        {
        }
    }

    return names;
}

/** @brief Parse a subject's string */
void Certificate::parseSubjectString(const std::string& subject_string, Subject& subject)
{
    std::vector<std::string> key_pairs = ocpp::helpers::split(subject_string, ',');
    for (const std::string& key_pair : key_pairs)
    {
        std::vector<std::string> key_val = ocpp::helpers::split(key_pair, '=');
        if (key_val.size() == 2)
        {
            ocpp::helpers::trim(key_val[0]);
            ocpp::helpers::trim(key_val[1]);
            if (key_val[0] == "C")
            {
                subject.country = key_val[1];
            }
            else if (key_val[0] == "ST")
            {
                subject.state = key_val[1];
            }
            else if (key_val[0] == "L")
            {
                subject.location = key_val[1];
            }
            else if (key_val[0] == "O")
            {
                subject.organization = key_val[1];
            }
            else if (key_val[0] == "OU")
            {
                subject.organization_unit = key_val[1];
            }
            else if (key_val[0] == "CN")
            {
                subject.common_name = key_val[1];
            }
            else if (key_val[0] == "emailAddress")
            {
                subject.email_address = key_val[1];
            }
            else
            {
            }
        }
    }
}

} // namespace websockets
} // namespace ocpp
