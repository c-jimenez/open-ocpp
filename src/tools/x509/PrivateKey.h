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

#ifndef PRIVATEKEY_H
#define PRIVATEKEY_H

#include <filesystem>
#include <string>

namespace ocpp
{
namespace x509
{

/** @brief Helper for private key manipulation */
class PrivateKey
{
  public:
    /** @brief Key type */
    enum Type
    {
        /** @brief RSA */
        RSA,
        /** @brief Elliptic Curve */
        EC
    };

    /**
     * @brief Elliptic Curves */
    enum Curve
    {
        /** @brief X9.62/SECG curve over a 256 bit prime field */
        PRIME256_V1,
        /** @brief SECG curve over a 256 bit prime field */
        SECP256_K1,
        /** @brief SECG curve over a 384 bit prime field */
        SECP384_R1,
        /** @brief SECG curve over a 521 bit prime field */
        SECP521_R1,
        /** @brief RFC 5639 curve over a 256 bit prime field */
        BRAINPOOLP256_T1,
        /** @brief RFC 5639 curve over a 384 bit prime field */
        BRAINPOOLP384_T1,
        /** @brief RFC 5639 curve over a 512 bit prime field */
        BRAINPOOLP512_T1
    };

    /**
     * @brief Constructor from PEM file
     * @param pem_file PEM file to load
     * @param passphrase Passphrase to decrypt the key (empty = no encryption)
     */
    PrivateKey(const std::filesystem::path& pem_file, const std::string& passphrase);

    /**
     * @brief Constructor from PEM data
     * @param pem_data PEM encoded data
     * @param passphrase Passphrase to decrypt the key (empty = no encryption)
     */
    PrivateKey(const std::string& pem_data, const std::string& passphrase);

    /**
     * @brief Constructor to generate a key
     * @param type Type of key
     * @param param Generation parameter : 
     *              \li For RSA : size of the key in bits
     *              \li For EC : Selected curve (Curve enum)
     * @param passphrase Passphrase to encrypt the key in PEM format (empty = no encryption)
     */
    PrivateKey(Type type, unsigned int param, const std::string& passphrase);

    /** @brief Destructor */
    virtual ~PrivateKey();

    /**
     * @brief Save the private key part as a PEM encoded file
     * @param pem_file Path of the file to generate
     * @return true if the private key has been saved, false otherwise
     */
    bool privateToFile(const std::filesystem::path& pem_file) const;

    /**
     * @brief Save the public key part as a PEM encoded file
     * @param pem_file Path of the file to generate
     * @return true if the public key has been saved, false otherwise
     */
    bool publicToFile(const std::filesystem::path& pem_file) const;

    /**
     * @brief Get the private key part as unencrypted PEM
     * @return Unencrypted PEM encoded data representation of the private key
     */
    std::string privatePemUnencrypted() const;

    /**
     * @brief Get the PEM encoded data representation of the private key
     * @return PEM encoded data representation of the private key
     */
    const std::string& privatePem() const { return m_private_pem; }

    /**
     * @brief Get the PEM encoded data representation of the public key
     * @return PEM encoded data representation of the public key
     */
    const std::string& publicPem() const { return m_public_pem; }

    /**
     * @brief Indicate if the key is valid
     * @return true if the key is valid, false otherwise
     */
    bool isValid() const { return m_is_valid; }

    /**
     * @brief Get the size of the key in bits
     * @return Size of the key in bits
     */
    unsigned int size() const { return m_size; }

    /** 
     * @brief Get the key algorithm
     * @return Key algorithm
     */
    const std::string& algo() const { return m_algo; }

    /** 
     * @brief Get the key algorithm parameter
     * @return Key algorithm parameter
     */
    const std::string& algoParam() const { return m_algo_param; }

    /**
     * @brief Get the underlying OpenSSL object
     * @return Underlying SSL object
     */
    const void* object() const { return m_openssl_object; }

  protected:
    /** @brief Indicate if the document is valid */
    bool m_is_valid;
    /** @brief PEM encoded data representation of the private key */
    std::string m_private_pem;
    /** @brief PEM encoded data representation of the public key */
    std::string m_public_pem;
    /** @brief Size of the key in bits */
    unsigned int m_size;
    /** @brief Key algorithm */
    std::string m_algo;
    /** @brief Key algorithm parameter */
    std::string m_algo_param;

    /** @brief Internal OpenSSL object */
    void* m_openssl_object;

    /** @brief Read the key from the PEM encoded data */
    void readKey(const std::string& passphrase);

    /** @brief Read key size and algorithm */
    void readKeySizeAlgo(void* pevp_pk);
};

} // namespace x509
} // namespace ocpp

#endif // PRIVATEKEY_H
