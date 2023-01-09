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

#include "PrivateKey.h"
#include "sign.h"
#include "openssl.h"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace ocpp
{
namespace x509
{

/** @brief NIDs of the EC curves */
static const int s_ec_curves_nids[] = {
    NID_X9_62_prime256v1, NID_secp256k1, NID_secp384r1, NID_secp521r1, NID_brainpoolP256t1, NID_brainpoolP384t1, NID_brainpoolP512t1};

/** @brief Constructor from PEM file */
PrivateKey::PrivateKey(const std::filesystem::path& pem_file, const std::string& passphrase)
    : m_is_valid(false), m_private_pem(), m_public_pem(), m_size(0), m_openssl_object(nullptr)
{
    // Open PEM file
    std::fstream file(pem_file, std::fstream::in | std::fstream::binary | std::fstream::ate);
    if (file.is_open())
    {
        // Read the whole file
        auto filesize = file.tellg();
        file.seekg(0, file.beg);
        m_private_pem.resize(filesize);
        file.read(&m_private_pem[0], filesize);

        // Read the key
        readKey(passphrase);
    }
}

/** @brief Constructor from PEM data */
PrivateKey::PrivateKey(const std::string& pem_data, const std::string& passphrase)
    : m_is_valid(false), m_private_pem(pem_data), m_public_pem(), m_size(0), m_openssl_object(nullptr)
{
    // Read the key
    readKey(passphrase);
}

/** @brief Constructor to generate a key */
PrivateKey::PrivateKey(Type type, unsigned int param, const std::string& passphrase)
    : m_is_valid(false), m_private_pem(), m_public_pem(), m_size(0), m_openssl_object(nullptr)
{
    EVP_PKEY*     pkey = nullptr;
    EVP_PKEY_CTX* ctx  = nullptr;

    // Key parametrization
    if (type == Type::EC)
    {
        ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
        if (ctx && (param < (sizeof(s_ec_curves_nids) / sizeof(int))))
        {
            EVP_PKEY_keygen_init(ctx);
            EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, s_ec_curves_nids[param]);
        }
    }
    else
    {
        ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        if (ctx)
        {
            EVP_PKEY_keygen_init(ctx);
            EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, param);
        }
    }
    if (ctx)
    {
        // Key generation
        EVP_PKEY_keygen(ctx, &pkey);

        // Release resources
        EVP_PKEY_CTX_free(ctx);

        // Validity
        m_is_valid = (pkey != nullptr);
        if (m_is_valid)
        {
            // Generate the PEM representations
            BIO*  bio      = nullptr;
            char* bio_data = nullptr;
            int   bio_len  = 0;

            // Private key
            bio = BIO_new(BIO_s_mem());
            if (passphrase.empty())
            {
                PEM_write_bio_PKCS8PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
            }
            else
            {
                char* pass = const_cast<char*>(passphrase.c_str());
                PEM_write_bio_PKCS8PrivateKey(bio, pkey, EVP_aes_256_cbc(), nullptr, 0, nullptr, pass);
            }
            bio_len = BIO_get_mem_data(bio, &bio_data);
            m_private_pem.insert(0, bio_data, static_cast<size_t>(bio_len));
            BIO_free(bio);

            // Public key
            bio = BIO_new(BIO_s_mem());
            PEM_write_bio_PUBKEY(bio, pkey);
            bio_len = BIO_get_mem_data(bio, &bio_data);
            m_public_pem.insert(0, bio_data, static_cast<size_t>(bio_len));
            BIO_free(bio);

            // Key size and algo
            readKeySizeAlgo(pkey);

            // Save OpenSSL object
            m_openssl_object = pkey;
        }
    }
}

/** @brief Copy constructor */
PrivateKey::PrivateKey(const PrivateKey& copy)
    : m_is_valid(false), m_private_pem(copy.privatePemUnencrypted()), m_public_pem(), m_size(0), m_openssl_object(nullptr)
{
    // Read the key
    readKey("");
}

/** @brief Destructor */
PrivateKey::~PrivateKey()
{
    EVP_PKEY_free(reinterpret_cast<EVP_PKEY*>(m_openssl_object));
}

/** @brief Compute the signature of a buffer using the private key */
std::vector<uint8_t> PrivateKey::sign(const void* buffer, size_t size, Sha2::Type sha) const
{
    EVP_PKEY* pkey = reinterpret_cast<EVP_PKEY*>(m_openssl_object);
    return ocpp::x509::sign(buffer, size, sha, pkey);
}

/** @brief Compute the signature of a file using the private key */
std::vector<uint8_t> PrivateKey::sign(const std::string& filepath, Sha2::Type sha) const
{
    EVP_PKEY* pkey = reinterpret_cast<EVP_PKEY*>(m_openssl_object);
    return ocpp::x509::sign(filepath, sha, pkey);
}

/** @brief Save the private key part as a PEM encoded file */
bool PrivateKey::privateToFile(const std::filesystem::path& pem_file) const
{
    bool         ret = false;
    std::fstream x509_file(pem_file, std::fstream::out);
    if (x509_file.is_open())
    {
        x509_file << m_private_pem;
        ret = true;
    }
    return ret;
}

/** @brief Save the public key part as a PEM encoded file */
bool PrivateKey::publicToFile(const std::filesystem::path& pem_file) const
{
    bool         ret = false;
    std::fstream x509_file(pem_file, std::fstream::out);
    if (x509_file.is_open())
    {
        x509_file << m_public_pem;
        ret = true;
    }
    return ret;
}

/** @brief Get the private key part as unencrypted PEM */
std::string PrivateKey::privatePemUnencrypted() const
{
    std::string pem;
    EVP_PKEY*   pkey = reinterpret_cast<EVP_PKEY*>(m_openssl_object);
    if (pkey)
    {
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PKCS8PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        char* bio_data = nullptr;
        int   bio_len  = BIO_get_mem_data(bio, &bio_data);
        pem.insert(0, bio_data, static_cast<size_t>(bio_len));
        BIO_free(bio);
    }
    return pem;
}

/** @brief Read the key from the PEM encoded data */
void PrivateKey::readKey(const std::string& passphrase)
{
    BIO* bio = BIO_new(BIO_s_mem());
    BIO_write(bio, m_private_pem.c_str(), static_cast<int>(m_private_pem.size()));
    char*     pass = const_cast<char*>(passphrase.c_str());
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, pass);
    BIO_free(bio);
    if (pkey)
    {
        // Public key
        bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bio, pkey);
        char* bio_data = nullptr;
        int   bio_len  = BIO_get_mem_data(bio, &bio_data);
        m_public_pem.insert(0, bio_data, static_cast<size_t>(bio_len));
        BIO_free(bio);

        // Key size and algo
        readKeySizeAlgo(pkey);

        // Save OpenSSL object
        m_is_valid       = true;
        m_openssl_object = pkey;
    }
    else
    {
        m_private_pem = "";
    }
}

/** @brief Read key size and algorithm */
void PrivateKey::readKeySizeAlgo(void* pevp_pk)
{
    EVP_PKEY* pkey = reinterpret_cast<EVP_PKEY*>(pevp_pk);

    // Key size
    m_size = static_cast<unsigned int>(EVP_PKEY_bits(pkey));

    // Algo
    int key_base_id  = EVP_PKEY_base_id(pkey);
    int key_type_nid = EVP_PKEY_type(key_base_id);
    m_algo           = OBJ_nid2sn(key_type_nid);
    if (key_base_id == EVP_PKEY_EC)
    {
#if (OPENSSL_VERSION_NUMBER < 0x30000000L)
        // OpenSSL 1.1.1.x
        const EC_KEY*   ec_key = EVP_PKEY_get0_EC_KEY(pkey);
        const EC_GROUP* group  = EC_KEY_get0_group(ec_key);
        m_algo_param           = OBJ_nid2sn(EC_GROUP_get_curve_name(group));
#else
        // OpenSSL 3.x
        char   curve_name[64];
        size_t len = 0;
        EVP_PKEY_get_utf8_string_param(pkey, OSSL_PKEY_PARAM_GROUP_NAME, curve_name, sizeof(curve_name), &len);
        curve_name[sizeof(curve_name) - 1u] = 0;
        m_algo_param                        = curve_name;
#endif // OPENSSL_VERSION_NUMBER
    }
}

} // namespace x509
} // namespace ocpp
