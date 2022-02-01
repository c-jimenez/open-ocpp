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

#ifndef CERTIFICATEREQUEST_H
#define CERTIFICATEREQUEST_H

#include "X509Document.h"

namespace ocpp
{
namespace x509
{

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

    /** @brief Destructor */
    virtual ~CertificateRequest();

    /** @brief Read X509 informations stored inside the certificate request */
    void readInfos();
};

} // namespace x509
} // namespace ocpp

#endif // CERTIFICATEREQUEST_H
