/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_CERTIFICATEREQUEST_H
#define OPENOCPP_CERTIFICATEREQUEST_H

#include "Sha2.h"
#include "X509Document.h"

namespace ocpp
{
namespace x509
{

class PrivateKey;

/** @brief Helper class for certificate request manipulation */
class CertificateRequest : public X509Document
{
  public:
    /**
     * @brief Constructor from PEM file
     * @param pem_file PEM file to load
     */
    CertificateRequest(const std::filesystem::path& pem_file);

    /**
     * @brief Constructor from PEM data
     * @param pem_data PEM encoded data
     */
    CertificateRequest(const std::string& pem_data);

    /**
     * @brief Constructor to generate a certificate request
     * @param subject Subject of the request
     * @param private_key Private key to use to sign the request
     * @param sha Secure hash algorithm to use to sign the request
     */
    CertificateRequest(const Subject& subject, const PrivateKey& private_key, Sha2::Type sha = Sha2::Type::SHA256);

    /**
     * @brief Constructor to generate a certificate request with extensions
     * @param subject Subject of the request
     * @param extensions X509v3 extensions
     * @param private_key Private key to use to sign the request
     * @param sha Secure hash algorithm to use to sign the request
     */
    CertificateRequest(const Subject&    subject,
                       const Extensions& extensions,
                       const PrivateKey& private_key,
                       Sha2::Type        sha = Sha2::Type::SHA256);

    /**
     * @brief Copy constructor
     * @param copy Certificate request to copy
     */
    CertificateRequest(const CertificateRequest& copy);

    /** @brief Destructor */
    virtual ~CertificateRequest();

  private:
    /** @brief Read X509 informations stored inside the certificate request */
    void readInfos();
    /** @brief Create a certificate request */
    void create(const Subject& subject, const Extensions& extensions, const PrivateKey& private_key, Sha2::Type sha);
};

} // namespace x509
} // namespace ocpp

#endif // OPENOCPP_CERTIFICATEREQUEST_H
