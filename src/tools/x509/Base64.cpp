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

#include "Base64.h"
#include "openssl.h"

namespace ocpp
{
namespace x509
{
namespace base64
{

/** @brief Encode an array of bytes in Base64 */
std::string encode(const void* data, size_t size)
{
    std::string b64_str;

    // Check input parameters
    if (data && (size != 0))
    {
        // Prepare buffer at the maximum data length
        b64_str.resize((4u * (size + 2u)) / 3u);

        // Encode data
        int encoded_size = EVP_EncodeBlock(
            reinterpret_cast<unsigned char*>(&b64_str[0]), reinterpret_cast<const unsigned char*>(data), static_cast<int>(size));
        if (encoded_size >= 0)
        {
            // Resize output
            b64_str.resize(static_cast<size_t>(encoded_size));
        }
        else
        {
            b64_str.clear();
        }
    }

    return b64_str;
}

/** @brief Decode a Base64 string into an array of bytes */
std::vector<uint8_t> decode(const std::string& b64_str)
{
    std::vector<uint8_t> data;

    // Check input parameters
    if (b64_str.size() != 0)
    {
        // Prepare buffer at the maximum data length
        data.resize((3u * b64_str.size()) / 4u);

        // Decode data
        int decoded_size =
            EVP_DecodeBlock(&data[0], reinterpret_cast<const unsigned char*>(b64_str.c_str()), static_cast<int>(b64_str.size()));
        if (decoded_size >= 0)
        {
            // Resize output
            if (b64_str[b64_str.size() - 1u] == '=')
            {
                decoded_size--;
            }
            if (b64_str[b64_str.size() - 2u] == '=')
            {
                decoded_size--;
            }
            data.resize(static_cast<size_t>(decoded_size));
        }
        else
        {
            data.clear();
        }
    }

    return data;
}

} // namespace base64
} // namespace x509
} // namespace ocpp
