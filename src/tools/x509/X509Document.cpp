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

#include "X509Document.h"
#include "String.h"

#include <cstring>
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
namespace x509
{

/** @brief Constructor from PEM file */
X509Document::X509Document(const std::filesystem::path& pem_file) : m_is_valid(false), m_pem()
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
    }
}

/** @brief Constructor from PEM data */
X509Document::X509Document(const std::string& pem_data) : m_is_valid(false), m_pem(pem_data) { }

/** @brief Destructor */
X509Document::~X509Document() { }

/** @brief Save the X509 document as a PEM encoded file */
bool X509Document::toFile(const std::filesystem::path& pem_file) const
{
    bool         ret = false;
    std::fstream x509_file(pem_file, x509_file.out);
    if (x509_file.is_open())
    {
        x509_file << m_pem;
        ret = true;
    }
    return ret;
}

/** @brief Parse a public key */
void X509Document::parsePublicKey(void* ppub_key)
{
    EVP_PKEY* pub_key      = reinterpret_cast<EVP_PKEY*>(ppub_key);
    int       key_base_id  = EVP_PKEY_base_id(pub_key);
    int       key_type_nid = EVP_PKEY_type(key_base_id);
    m_pub_key_algo         = OBJ_nid2sn(key_type_nid);
    m_pub_key_size         = static_cast<unsigned int>(EVP_PKEY_bits(pub_key));
    if (key_base_id == EVP_PKEY_EC)
    {
        EC_KEY*         ec_key = EVP_PKEY_get0_EC_KEY(pub_key);
        const EC_GROUP* group  = EC_KEY_get0_group(ec_key);
        m_pub_key_algo_param   = OBJ_nid2sn(EC_GROUP_get_curve_name(group));
    }

    X509_PUBKEY* x509_pub_key = nullptr;
    X509_PUBKEY_set(&x509_pub_key, pub_key);
    int                  pklen = 0;
    const unsigned char* k     = nullptr;
    X509_PUBKEY_get0_param(nullptr, &k, &pklen, nullptr, x509_pub_key);

    std::stringstream ss_pubkey;
    ss_pubkey << std::hex;
    for (int i = 0; i < pklen; i++)
    {
        ss_pubkey << std::setw(2) << std::setfill('0') << static_cast<int>(k[i]) << ":";
        m_pub_key.push_back(k[i]);
    }
    m_pub_key_string = ss_pubkey.str();
    m_pub_key_string.resize(m_pub_key_string.size() - 1u);

    X509_PUBKEY_free(x509_pub_key);
}

/** @brief Convert a date in ASN1_TIME format to a standard time_t representation */
time_t X509Document::convertAsn1Time(const void* pasn1_time)
{
    const ASN1_TIME* asn1_time = reinterpret_cast<const ASN1_TIME*>(pasn1_time);
    struct tm        tm;
    ASN1_TIME_to_tm(asn1_time, &tm);
    return mktime(&tm);
}

/** @brief Convert a string in ASN1_STRING format to a standard string representation */
std::string X509Document::convertAsn1String(const void* pasn1_string)
{
    std::string        str;
    const ASN1_STRING* asn1_string = reinterpret_cast<const ASN1_STRING*>(pasn1_string);
    if (ASN1_STRING_type(asn1_string) != V_ASN1_UTF8STRING)
    {
        char* c_str = nullptr;
        ASN1_STRING_to_UTF8((unsigned char**)&c_str, asn1_string);
        str = c_str;
        OPENSSL_free(c_str);
    }
    else
    {
        str.insert(
            0, reinterpret_cast<const char*>(ASN1_STRING_get0_data(asn1_string)), static_cast<size_t>(ASN1_STRING_length(asn1_string)));
    }
    return str;
}

/** @brief Convert a string in X509_NAME format to a standard string representation */
std::string X509Document::convertX509Name(const void* px509_name)
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
std::vector<std::string> X509Document::convertGeneralNames(const void* pgeneral_names)
{
    std::vector<std::string> names;
    const STACK_OF(GENERAL_NAME)* general_names = reinterpret_cast<const STACK_OF(GENERAL_NAME)*>(pgeneral_names);

    int names_count = sk_GENERAL_NAME_num(general_names);
    for (int i = 0; i < names_count; i++)
    {
        const GENERAL_NAME* current_name = sk_GENERAL_NAME_value(general_names, i);
        if (current_name->type == GEN_DNS)
        {
            names.emplace_back(convertAsn1String(current_name->d.dNSName));
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
            names.emplace_back(convertAsn1String(current_name->d.dNSName));
        }
        else
        {
        }
    }

    return names;
}

/** @brief Parse a subject's string */
void X509Document::parseSubjectString(const void* px509_name, Subject& subject)
{
    const X509_NAME* x509_name = reinterpret_cast<const X509_NAME*>(px509_name);
    for (int i = 0; i < X509_NAME_entry_count(x509_name); i++)
    {
        X509_NAME_ENTRY* entry        = X509_NAME_get_entry(x509_name, i);
        ASN1_STRING*     entry_data   = X509_NAME_ENTRY_get_data(entry);
        ASN1_OBJECT*     entry_object = X509_NAME_ENTRY_get_object(entry);
        const char*      key_name     = OBJ_nid2sn(OBJ_obj2nid(entry_object));
        std::string      value        = convertAsn1String(entry_data);
        if (strcmp(key_name, "C") == 0)
        {
            subject.country = value;
        }
        else if (strcmp(key_name, "ST") == 0)
        {
            subject.state = value;
        }
        else if (strcmp(key_name, "L") == 0)
        {
            subject.location = value;
        }
        else if (strcmp(key_name, "O") == 0)
        {
            subject.organization = value;
        }
        else if (strcmp(key_name, "OU") == 0)
        {
            subject.organization_unit = value;
        }
        else if (strcmp(key_name, "CN") == 0)
        {
            subject.common_name = value;
        }
        else if (strcmp(key_name, "emailAddress") == 0)
        {
            subject.email_address = value;
        }
        else
        {
        }
    }
}

} // namespace x509
} // namespace ocpp
