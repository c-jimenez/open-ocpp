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

#include "Sha2.h"
#include "openssl.h"

#include <iomanip>
#include <sstream>

namespace ocpp
{
namespace x509
{

/** @brief Constructor */
Sha2::Sha2(Type type) : m_context(nullptr), m_md(nullptr), m_last_result()
{
    // Allocate computation context
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_MD_CTX_init(ctx);
    if (type == Type::SHA256)
    {
        m_last_result.resize(SHA256_DIGEST_LENGTH);
        m_md = EVP_sha256();
    }
    else if (type == Type::SHA384)
    {
        m_last_result.resize(SHA384_DIGEST_LENGTH);
        m_md = EVP_sha384();
    }
    else
    {
        m_last_result.resize(SHA512_DIGEST_LENGTH);
        m_md = EVP_sha512();
    }
    m_context = ctx;
}

/** @brief Destructor */
Sha2::~Sha2()
{
    EVP_MD_CTX_free(reinterpret_cast<EVP_MD_CTX*>(m_context));
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
    EVP_DigestInit(reinterpret_cast<EVP_MD_CTX*>(m_context), reinterpret_cast<const EVP_MD*>(m_md));
}

/** @brief Add data to the current computation */
void Sha2::update(const void* data, size_t size)
{
    EVP_DigestUpdate(reinterpret_cast<EVP_MD_CTX*>(m_context), data, size);
}

/** @brief Finalize the computation */
std::vector<uint8_t> Sha2::finalize()
{
    unsigned int size = static_cast<unsigned int>(m_last_result.size());
    EVP_DigestFinal(reinterpret_cast<EVP_MD_CTX*>(m_context), &m_last_result[0], &size);
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
