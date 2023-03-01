/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_SIGN_H
#define OPENOCPP_SIGN_H

#include "Sha2.h"

#include <openssl/x509.h>

namespace ocpp
{
namespace x509
{

/**
 * @brief Compute the signature of a buffer using a key
 * @param buffer Buffer to use
 * @param size Size in bytes of the buffer
 * @param sha Secure hash algorithm to use
 * @param pkey Key to use
 * @return Computed signature or empty vector on error
 */
std::vector<uint8_t> sign(const void* buffer, size_t size, Sha2::Type sha, EVP_PKEY* pkey);

/**
 * @brief Compute the signature of a file using a key
 * @param filepath Path to the file
 * @param sha Secure hash algorithm to use
 * @param pkey Key to use
 * @return Computed signature or empty vector on error
 */
std::vector<uint8_t> sign(const std::string& filepath, Sha2::Type sha, EVP_PKEY* pkey);

/**
 * @brief Verify the signature of a buffer using a key
 * @param signature Expected signature
 * @param buffer Buffer to use
 * @param size Size in bytes of the buffer
 * @param sha Secure hash algorithm to use
 * @param pkey Key to use
 * @return true is the signature is valid, false otherwise
 */
bool verify(const std::vector<uint8_t>& signature, const void* buffer, size_t size, Sha2::Type sha, EVP_PKEY* pkey);

/**
 * @brief Verify the signature of a file using a key
 * @param signature Expected signature
 * @param filepath Path to the file
 * @param sha Secure hash algorithm to use
 * @param pkey Key to use
 * @return true is the signature is valid, false otherwise
 */
bool verify(const std::vector<uint8_t>& signature, const std::string& filepath, Sha2::Type sha, EVP_PKEY* pkey);

} // namespace x509
} // namespace ocpp

#endif // OPENOCPP_SIGN_H
