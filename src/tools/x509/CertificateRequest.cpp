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

#include "CertificateRequest.h"
#include "PrivateKey.h"

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
CertificateRequest::CertificateRequest(const std::filesystem::path& pem_file) : X509Document(pem_file)
{
    // Read PEM infos
    readInfos();
}

/** @brief Constructor from PEM data */
CertificateRequest::CertificateRequest(const std::string& pem_data) : X509Document(pem_data)
{
    // Read PEM infos
    readInfos();
}

/** @brief Constructor to generate a certificate request */
CertificateRequest::CertificateRequest(const Subject& subject, const PrivateKey& private_key, Sha2::Type sha)
    : X509Document(std::string(""))
{
    // Check key validity
    if (!private_key.isValid())
    {
        return;
    }

    // Create X509 request
    X509_REQ* x509_req = X509_REQ_new();

    // Set version (v3)
    X509_REQ_set_version(x509_req, 2);

    // Add subject
    X509_NAME* subject_name = X509_NAME_new();
    if (!subject.country.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_countryName,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.country.c_str()),
                                   subject.country.size(),
                                   -1,
                                   0);
    }
    if (!subject.state.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_stateOrProvinceName,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.state.c_str()),
                                   subject.state.size(),
                                   -1,
                                   0);
    }
    if (!subject.location.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_localityName,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.location.c_str()),
                                   subject.location.size(),
                                   -1,
                                   0);
    }
    if (!subject.organization.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_organizationName,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.organization.c_str()),
                                   subject.organization.size(),
                                   -1,
                                   0);
    }
    if (!subject.organization_unit.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_organizationalUnitName,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.organization_unit.c_str()),
                                   subject.organization_unit.size(),
                                   -1,
                                   0);
    }
    if (!subject.common_name.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_commonName,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.common_name.c_str()),
                                   subject.common_name.size(),
                                   -1,
                                   0);
    }
    if (!subject.email_address.empty())
    {
        X509_NAME_add_entry_by_NID(subject_name,
                                   NID_pkcs9_emailAddress,
                                   V_ASN1_UTF8STRING,
                                   reinterpret_cast<const unsigned char*>(subject.email_address.c_str()),
                                   subject.email_address.size(),
                                   -1,
                                   0);
    }
    X509_REQ_set_subject_name(x509_req, subject_name);
    X509_NAME_free(subject_name);

    // Set key
    EVP_PKEY* pkey = const_cast<EVP_PKEY*>(reinterpret_cast<const EVP_PKEY*>(private_key.object()));
    X509_REQ_set_pubkey(x509_req, pkey);

    // Sign request
    const EVP_MD* digest = nullptr;
    if (sha == Sha2::Type::SHA256)
    {
        digest = EVP_get_digestbynid(NID_sha256);
    }
    else if (sha == Sha2::Type::SHA256)
    {
        digest = EVP_get_digestbynid(NID_sha384);
    }
    else
    {
        digest = EVP_get_digestbynid(NID_sha512);
    }
    X509_REQ_sign(x509_req, pkey, digest);

    // Convert to PEM
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_X509_REQ(bio, x509_req);
    char* bio_data = nullptr;
    int   bio_len  = BIO_get_mem_data(bio, &bio_data);
    m_pem.insert(0, bio_data, static_cast<size_t>(bio_len));
    BIO_free(bio);

    // Release memory
    X509_REQ_free(x509_req);

    // Read PEM infos
    readInfos();
}

/** @brief Destructor */
CertificateRequest::~CertificateRequest() { }

/** @brief Read X509 informations stored inside the certificate request */
void CertificateRequest::readInfos()
{
    // Load PEM
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, m_pem.c_str(), static_cast<int>(m_pem.size()));
    X509_REQ* cert_request = PEM_read_bio_X509_REQ(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (cert_request)
    {
        // Certificate request is valid
        m_is_valid = true;

        // Extract subject
        X509_NAME* subject = X509_REQ_get_subject_name(cert_request);
        m_subject_string   = convertX509Name(subject);
        parseSubjectString(subject, m_subject);

        // Extract signature algorithm name
        const ASN1_BIT_STRING* sig = nullptr;
        const X509_ALGOR*      alg = nullptr;
        X509_REQ_get0_signature(cert_request, &sig, &alg);

        int sig_nid = OBJ_obj2nid(alg->algorithm);
        m_sig_hash  = OBJ_nid2sn(sig_nid);
        m_sig_algo  = OBJ_nid2sn(X509_REQ_get_signature_nid(cert_request));

        // Extract public key infos
        EVP_PKEY* pub_key_cert = X509_REQ_get0_pubkey(cert_request);
        parsePublicKey(pub_key_cert);

        X509_REQ_free(cert_request);
    }
}

} // namespace x509
} // namespace ocpp
