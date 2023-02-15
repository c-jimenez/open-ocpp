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

#ifndef OPENOCPP_BASE64_H
#define OPENOCPP_BASE64_H

#include <cstdint>
#include <string>
#include <vector>

namespace ocpp
{
namespace x509
{
namespace base64
{

/**
 * @brief Encode an array of bytes in Base64
 * @param data Data to encode
 * @param size Size in bytes of the data to encode
 * @return Base64 representation of the input array
 */
std::string encode(const void* data, size_t size);

/**
 * @brief Decode a Base64 string into an array of bytes
 * @param b64_str  Base64 string to decode
 * @return Corresponding array of bytes or empty array if the string
 *         format is invalid
 */
std::vector<uint8_t> decode(const std::string& b64_str);

} // namespace base64
} // namespace x509
} // namespace ocpp

#endif // OPENOCPP_BASE64_H
