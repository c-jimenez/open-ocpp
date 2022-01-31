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

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace ocpp
{
namespace websockets
{

/** @brief Helper class for certificate manipulation */
class Certificate
{
  public:
    struct Subject;

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

    /** @brief Destructor */
    virtual ~Certificate();

    /**
     * @brief Indicate if the certificate is valid
     * @return true if the certificate is valid, false otherwise
     */
    bool isValid() const { return m_is_valid; }

    /**
     * @brief Get the PEM encoded data representation of the certificate
     * @return PEM encoded data representation of the certificate
     */
    const std::string& pem() const { return m_pem; }

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
    const std::vector<std::string>& issuerAltNames() const { return m_issuer_alternate_names; }

    /** 
     * @brief Get the subject 
     * @return Subject
     */
    const Subject& subject() const { return m_subject; }

    /** 
     * @brief Get the subject string
     * @return Subject string
     */
    const std::string& subjectString() const { return m_subject_string; }

    /** 
     * @brief Get the subject alternate names
     * @return Subject alternate names
     */
    const std::vector<std::string>& subjectAltNames() const { return m_subject_alternate_names; }

    /** 
     * @brief Get the signature algorithm 
     * @return Signature algorithm
     */
    const std::string& signatureAlgo() const { return m_sig_algo; }

    /** 
     * @brief Get the signature hash 
     * @return Signature hash
     */
    const std::string& signatureHash() const { return m_sig_hash; }

    /** 
     * @brief Get the public key 
     * @return Public key
     */
    const std::vector<uint8_t>& publicKey() const { return m_pub_key; }

    /** 
     * @brief Get the public key as string
     * @return Public key as string
     */
    const std::string& publicKeyString() const { return m_pub_key_string; }

    /** 
     * @brief Get the public key algorithm
     * @return Public key algorithm
     */
    const std::string& publicKeyAlgo() const { return m_pub_key_algo; }

    /** 
     * @brief Get the public key algorithm parameter
     * @return Public key algorithm parameter
     */
    const std::string& publicKeyAlgoParam() const { return m_pub_key_algo_param; }

    /** 
     * @brief Get the X509v3 extensions
     * @return X509v3 extensions
     */
    const std::vector<std::string>& x509v3Extensions() const { return m_x509v3_extensions; }

    /** @brief Contains subject information */
    struct Subject
    {
        /** @brief Country */
        std::string country;
        /** @brief State */
        std::string state;
        /** @brief Location */
        std::string location;
        /** @brief Organization */
        std::string organization;
        /** @brief Organization unit */
        std::string organization_unit;
        /** @brief Common name */
        std::string common_name;
        /** @brief E-mail address */
        std::string email_address;
    };

  private:
    /** @brief Indicate if the certificate is valid */
    bool m_is_valid;
    /** @brief PEM encoded data representation of the certificate */
    std::string m_pem;
    /** @brief PEM encoded data representation of each certificate composing the certificate chain (if any) */
    std::vector<std::string> m_pem_chain;
    /** @brief Certificates composing the certificate chain (if any) */
    std::vector<Certificate> m_certificate_chain;

    /** @brief Serial number */
    std::vector<uint8_t> m_serial_number;
    /** @brief Serial number as string */
    std::string m_serial_number_string;
    /** @brief Date of start of validity */
    time_t m_validity_from;
    /** @brief Date of end of validity */
    time_t m_validity_to;
    /** @brief Issuer */
    Subject m_issuer;
    /** @brief Issuer string */
    std::string m_issuer_string;
    /** @brief Issuer alternate names */
    std::vector<std::string> m_issuer_alternate_names;
    /** @brief Subject */
    Subject m_subject;
    /** @brief Subject string */
    std::string m_subject_string;
    /** @brief Subject alternate names */
    std::vector<std::string> m_subject_alternate_names;
    /** @brief Signature algorithm */
    std::string m_sig_algo;
    /** @brief Signature hash */
    std::string m_sig_hash;
    /** @brief Public key */
    std::vector<uint8_t> m_pub_key;
    /** @brief Public key as hexadecimal string */
    std::string m_pub_key_string;
    /** @brief Public key algorithm */
    std::string m_pub_key_algo;
    /** @brief Public key algorithm parameter */
    std::string m_pub_key_algo_param;
    /** @brief X509v3 extensions */
    std::vector<std::string> m_x509v3_extensions;

    /** @brief Extract all the PEM certificates in the certificate chain */
    void extractPemChain();

    /** @brief Read X509 informations stored inside a certificate */
    static void readInfos(Certificate& certificate);

    /** @brief Convert a date in ASN1_TIME format to a standard time_t representation */
    static time_t convertAsn1Time(const void* pasn1_time);
    /** @brief Convert a string in X509_NAME format to a standard string representation */
    static std::string convertX509Name(const void* px509_name);
    /** @brief Convert a list of strings in GENERAL_NAMES format to a standard vector of strings representation */
    static std::vector<std::string> convertGeneralNames(const void* pgeneral_names);

    /** @brief Parse a subject's string */
    static void parseSubjectString(const std::string& subject_string, Subject& subject);
};

} // namespace websockets
} // namespace ocpp

#endif // CERTIFICATE_H
