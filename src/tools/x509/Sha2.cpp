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

#include "Sha2.h"

#include <iomanip>
#include <sstream>

#include <openssl/sha.h>

namespace ocpp
{
namespace x509
{

/** @brief Constructor */
Sha2::Sha2(Type type) : m_type(type), m_context(nullptr), m_last_result()
{
    // Allocate computation context
    if (type == Type::SHA256)
    {
        m_last_result.resize(SHA256_DIGEST_LENGTH);
        m_context = new SHA256_CTX();
    }
    else if (type == Type::SHA384)
    {
        m_last_result.resize(SHA384_DIGEST_LENGTH);
        m_context = new SHA512_CTX();
    }
    else
    {
        m_last_result.resize(SHA512_DIGEST_LENGTH);
        m_context = new SHA512_CTX();
    }
}

/** @brief Destructor */
Sha2::~Sha2()
{
    // void* cannot be deleted using 'delete'
    // so cast into int* to free memory without destructor call
    delete reinterpret_cast<int*>(m_context);
}

/** @brief Compute the SHA of a data buffer */
std::vector<uint8_t> Sha2::compute(const void* data, size_t size)
{
    init();
    update(data, size);
    return finalize();
}

/** @brief Initialize a new computation */
void Sha2::init()
{
    if (m_type == Type::SHA256)
    {
        SHA256_Init(reinterpret_cast<SHA256_CTX*>(m_context));
    }
    else if (m_type == Type::SHA384)
    {
        SHA384_Init(reinterpret_cast<SHA512_CTX*>(m_context));
    }
    else
    {
        SHA512_Init(reinterpret_cast<SHA512_CTX*>(m_context));
    }
}

/** @brief Add data to the current computation */
void Sha2::update(const void* data, size_t size)
{
    if (m_type == Type::SHA256)
    {
        SHA256_Update(reinterpret_cast<SHA256_CTX*>(m_context), data, size);
    }
    else if (m_type == Type::SHA384)
    {
        SHA384_Update(reinterpret_cast<SHA512_CTX*>(m_context), data, size);
    }
    else
    {
        SHA512_Update(reinterpret_cast<SHA512_CTX*>(m_context), data, size);
    }
}

/** @brief Finalize the computation */
std::vector<uint8_t> Sha2::finalize()
{
    if (m_type == Type::SHA256)
    {
        SHA256_Final(&m_last_result[0], reinterpret_cast<SHA256_CTX*>(m_context));
    }
    else if (m_type == Type::SHA384)
    {
        SHA384_Final(&m_last_result[0], reinterpret_cast<SHA512_CTX*>(m_context));
    }
    else
    {
        SHA512_Final(&m_last_result[0], reinterpret_cast<SHA512_CTX*>(m_context));
    }

    return m_last_result;
}

/** @brief Get the last computation result as an hexadecimal string */
std::string Sha2::resultString() const
{
    std::stringstream ss;
    ss << std::hex;
    for (uint8_t val : m_last_result)
    {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(val) << "";
    }
    return ss.str();
}

} // namespace x509
} // namespace ocpp
