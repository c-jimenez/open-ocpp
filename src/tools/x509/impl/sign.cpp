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

#include "sign.h"
#include "openssl.h"

#include <fstream>
#include <vector>

namespace ocpp
{
namespace x509
{

/** @brief Get the corresponding OpenSSL hash algorithm */
static const EVP_MD* getHash(Sha2::Type sha);

/** @brief Compute the signature of a buffer using a key */
std::vector<uint8_t> sign(const void* buffer, size_t size, Sha2::Type sha, EVP_PKEY* pkey)
{
    std::vector<uint8_t> signature;
    if (buffer && pkey)
    {
        // Initialize signing context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_MD_CTX_init(ctx);

        // Select hash algorithm
        const EVP_MD* md = getHash(sha);

        // Init signature computation
        EVP_DigestSignInit(ctx, nullptr, md, nullptr, pkey);

        // Compute digest
        EVP_DigestSignUpdate(ctx, buffer, size);

        // Compute signature
        signature.resize(static_cast<size_t>(EVP_PKEY_size(pkey)));
        size_t sig_size = signature.size();
        if (EVP_DigestSignFinal(ctx, &signature[0], &sig_size) == 1)
        {
            signature.resize(sig_size);
        }
        else
        {
            signature.clear();
        }

        // Release resources
        EVP_MD_CTX_free(ctx);
    }
    return signature;
}

/** @brief Compute the signature of a file using a key */
std::vector<uint8_t> sign(const std::string& filepath, Sha2::Type sha, EVP_PKEY* pkey)
{
    std::vector<uint8_t> signature;
    if (pkey)
    {
        // Open the file
        std::fstream file(filepath, std::fstream::in | std::fstream::binary);
        if (file.is_open())
        {
            // Initialize signing context
            EVP_MD_CTX* ctx = EVP_MD_CTX_new();
            EVP_MD_CTX_init(ctx);

            // Select hash algorithm
            const EVP_MD* md = getHash(sha);

            // Init signature computation
            EVP_DigestSignInit(ctx, nullptr, md, nullptr, pkey);

            // Compute digest
            uint8_t buffer[1024];
            do
            {
                file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
                EVP_DigestSignUpdate(ctx, buffer, static_cast<size_t>(file.gcount()));
            } while (file.gcount() == sizeof(buffer));

            // Compute signature
            signature.resize(static_cast<size_t>(EVP_PKEY_size(pkey)));
            size_t sig_size = signature.size();
            if (EVP_DigestSignFinal(ctx, &signature[0], &sig_size) == 1)
            {
                signature.resize(sig_size);
            }
            else
            {
                signature.clear();
            }

            // Release resources
            EVP_MD_CTX_free(ctx);
        }
    }
    return signature;
}

/** @brief Verify the signature of a buffer using a key */
bool verify(const std::vector<uint8_t>& signature, const void* buffer, size_t size, Sha2::Type sha, EVP_PKEY* pkey)
{
    bool ret = false;
    if (!signature.empty() && buffer && pkey)
    {
        // Initialize verify context
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_MD_CTX_init(ctx);

        // Select hash algorithm
        const EVP_MD* md = getHash(sha);

        // Init signature verification
        EVP_DigestVerifyInit(ctx, nullptr, md, nullptr, pkey);

        // Compute digest
        EVP_DigestVerifyUpdate(ctx, buffer, size);

        // Verify signature
        ret = (EVP_DigestVerifyFinal(ctx, &signature[0], signature.size()) == 1);

        // Release resources
        EVP_MD_CTX_free(ctx);
    }
    return ret;
}

/**
 * @brief Verify the signature of a file using a key
 * @param signature Expected signature
 * @param filepath Path to the file
 * @param sha Secure hash algorithm to use
 * @param pkey Key to use
 * @return true is the signature is valid, false otherwise
 */
bool verify(const std::vector<uint8_t>& signature, const std::string& filepath, Sha2::Type sha, EVP_PKEY* pkey)
{
    bool ret = false;
    if (!signature.empty() && pkey)
    {
        // Open the file
        std::fstream file(filepath, std::fstream::in | std::fstream::binary);
        if (file.is_open())
        {
            // Initialize verify context
            EVP_MD_CTX* ctx = EVP_MD_CTX_new();
            EVP_MD_CTX_init(ctx);

            // Select hash algorithm
            const EVP_MD* md = getHash(sha);

            // Init signature verification
            EVP_DigestVerifyInit(ctx, nullptr, md, nullptr, pkey);

            // Compute digest
            uint8_t buffer[1024];
            do
            {
                file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
                EVP_DigestVerifyUpdate(ctx, buffer, static_cast<size_t>(file.gcount()));
            } while (file.gcount() == sizeof(buffer));

            // Verify signature
            ret = (EVP_DigestVerifyFinal(ctx, &signature[0], signature.size()) == 1);

            // Release resources
            EVP_MD_CTX_free(ctx);
        }
    }
    return ret;
}

/** @brief Get the corresponding OpenSSL hash algorithm */
static const EVP_MD* getHash(Sha2::Type sha)
{
    const EVP_MD* md = nullptr;
    switch (sha)
    {
        default:
        // Intended fallthrough
        case Sha2::Type::SHA256:
            md = EVP_sha256();
            break;
        case Sha2::Type::SHA384:
            md = EVP_sha384();
            break;
        case Sha2::Type::SHA512:
            md = EVP_sha512();
            break;
    }
    return md;
}

} // namespace x509
} // namespace ocpp
