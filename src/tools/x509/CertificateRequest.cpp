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
