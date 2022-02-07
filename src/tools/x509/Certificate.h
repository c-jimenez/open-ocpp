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

#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "Sha2.h"
#include "X509Document.h"

namespace ocpp
{
namespace x509
{

class PrivateKey;
class CertificateRequest;

/** @brief Helper class for certificate manipulation */
class Certificate : public X509Document
{
  public:
    /**
     * @brief Constructor from PEM file
     * @param pem_file PEM file to load
     */
    Certificate(const std::filesystem::path& pem_file);

    /**
     * @brief Constructor from PEM data
     * @param pem_data PEM encoded data
     */
    Certificate(const std::string& pem_data);

    /**
     * @brief Constructor from certificate request and signing certificate
     * @param certificate_request Certificate request to be signed
     * @param signing_certificate Certificate which will sign the request
     * @param private_key Private key of the certificate which will sign the request
     * @param sha Secure hash algorithm to use to sign the request 
     * @param days New certficate validity in days
     */
    Certificate(const CertificateRequest& certificate_request,
                const Certificate&        signing_certificate,
                const PrivateKey&         private_key,
                Sha2::Type                sha,
                unsigned int              days);

    /**
     * @brief Constructor for a self-signed certificate from a certificate request
     * @param certificate_request Certificate request to be signed
     * @param private_key Private key of the certificate request
     * @param sha Secure hash algorithm to use to sign the request 
     * @param days New certficate validity in days
     */
    Certificate(const CertificateRequest& certificate_request, const PrivateKey& private_key, Sha2::Type sha, unsigned int days);

    /**
     * @brief Copy constructor
     * @param copy Certificate to copy
     */
    Certificate(const Certificate& copy);

    /** @brief Destructor */
    virtual ~Certificate();

    /**
     * @brief Verify the PEM certificate chain
     *        The certificate to verify must be the first in list, then the sub-CAs
     *        if they exists and finally the root-CA
     * @return true if the PEM certificate chain is valid, false otherwise
     */
    bool verify() const;

    /**
     * @brief Verify the certificate signature against a certificate chain
     *        The certificate chain must be ordered by the sub-CAs
     *        if they exists and finally the root-CA
     * @param ca_chain Certificate chain to use
     * @return true if certificate signature is valid, false otherwise
     */
    bool verify(const std::vector<Certificate>& ca_chain) const;

    /** 
     * @brief Verify the signature of a buffer using the certificate's public key
     * @param signature Expected signature
     * @param buffer Buffer to use
     * @param size Size in bytes of the buffer
     * @param sha Secure hash algorithm to use
     * @return true is the signature is valid, false otherwise
     */
    bool verify(const std::vector<uint8_t>& signature, const void* buffer, size_t size, Sha2::Type sha = Sha2::Type::SHA256);

    /** 
     * @brief Verify the signature of a file using the certificate's public key
     * @param signature Expected signature
     * @param filepath Path to the file
     * @param sha Secure hash algorithm to use
     * @return true is the signature is valid, false otherwise
     */
    bool verify(const std::vector<uint8_t>& signature, const std::string& filepath, Sha2::Type sha);

    /** 
     * @brief Get the PEM encoded data representation of each certificate composing the certificate chain (if any) 
     * @return PEM encoded data representation of each certificate composing the certificate chain (if any)
     */
    const std::vector<std::string>& pemChain() const { return m_pem_chain; }

    /** @brief Get the certificates composing the certificate chain (if any)
     *  @return Certificates composing the certificate chain (if any) 
     */
    const std::vector<Certificate>& certificateChain() const { return m_certificate_chain; }

    /** 
     * @brief Get the serial number 
     * @return Serial number
     */
    const std::vector<uint8_t>& serialNumber() const { return m_serial_number; }

    /** 
     * @brief Get the serial number as string
     * @return Serial number as string
     */
    const std::string& serialNumberString() const { return m_serial_number_string; }

    /** 
     * @brief Get the serial number as an hex string
     * @return Serial number as an hex string
     */
    const std::string& serialNumberHexString() const { return m_serial_number_hex_string; }

    /** 
     * @brief Get the date of start of validity 
     * @return Date of start of validity
     */
    time_t validityFrom() const { return m_validity_from; }

    /** 
     * @brief Get the date of end of validity 
     * @return Date of end of validity
     */
    time_t validityTo() const { return m_validity_to; }

    /** 
     * @brief Get the issuer 
     * @return Issuer
     */
    const Subject& issuer() const { return m_issuer; }

    /** 
     * @brief Get the issuer string
     * @return Issuer string
     */
    const std::string& issuerString() const { return m_issuer_string; }

    /** 
     * @brief Get the issuer alternate names
     * @return Issuer alternate names
     */
    const std::vector<std::string>& issuerAltNames() const { return m_x509v3_extensions.issuer_alternate_names; }

    /** 
     * @brief Indicate if it is a self-signed certificate 
     * @return true if it is a self signed certificate, false otherwise
     */
    bool isSelfSigned() const { return m_is_self_signed; }

  private:
    /** @brief PEM encoded data representation of each certificate composing the certificate chain (if any) */
    std::vector<std::string> m_pem_chain;
    /** @brief Certificates composing the certificate chain (if any) */
    std::vector<Certificate> m_certificate_chain;

    /** @brief Serial number */
    std::vector<uint8_t> m_serial_number;
    /** @brief Serial number as string */
    std::string m_serial_number_string;
    /** @brief Serial number as an hex string */
    std::string m_serial_number_hex_string;
    /** @brief Date of start of validity */
    time_t m_validity_from;
    /** @brief Date of end of validity */
    time_t m_validity_to;
    /** @brief Issuer */
    Subject m_issuer;
    /** @brief Issuer string */
    std::string m_issuer_string;
    /** @brief Indicate if it is a self-signed certificate */
    bool m_is_self_signed;

    /** @brief Extract all the PEM certificates in the certificate chain */
    void extractPemChain();
    /** @brief Converts a certificate request to a certificate */
    void convertCertificateRequest(void* request, const void* issuer, void* key, Sha2::Type sha, unsigned int days);

    /** @brief Load OpenSSL X509 certificate structure from a PEM encoded data string */
    static void* loadX509(const std::string& pem_data);
    /** @brief Read X509 informations stored inside a certificate */
    static void readInfos(Certificate& certificate);

    /** @brief Verify a certificate against a chain of certificates */
    static bool verify(const Certificate& certificate, const std::vector<Certificate>& certificate_chain, size_t start_index);
};

} // namespace x509
} // namespace ocpp

#endif // CERTIFICATE_H
