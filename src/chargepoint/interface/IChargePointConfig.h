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

#ifndef OPENOCPP_ICHARGEPOINTCONFIG_H
#define OPENOCPP_ICHARGEPOINTCONFIG_H

#include <chrono>
#include <string>

namespace ocpp
{
namespace config
{

/** @brief Interface to retrieve stack internal configuration for the Charge Point role */
class IChargePointConfig
{
  public:
    /** @brief Destructor */
    virtual ~IChargePointConfig() { }

    // Paths

    /** @brief Path to the database to store persistent data */
    virtual std::string databasePath() const = 0;
    /** @brief Path to the JSON schemas to validate the messages */
    virtual std::string jsonSchemasPath() const = 0;

    // Communication parameters

    /** @brief Connection URL */
    virtual std::string connexionUrl() const = 0;
    /** @brief Charge point identifier */
    virtual std::string chargePointIdentifier() const = 0;
    /** @brief Connection timeout */
    virtual std::chrono::milliseconds connectionTimeout() const = 0;
    /** @brief Retry interval */
    virtual std::chrono::milliseconds retryInterval() const = 0;
    /** @brief Call request timeout */
    virtual std::chrono::milliseconds callRequestTimeout() const = 0;
    /** @brief Cipher list to use for TLSv1.2 connections */
    virtual std::string tlsv12CipherList() const = 0;
    /** @brief Cipher list to use for TLSv1.3 connections */
    virtual std::string tlsv13CipherList() const = 0;
    /** @brief Certification Authority signing chain for the server certificate */
    virtual std::string tlsServerCertificateCa() const = 0;
    /** @brief Client certificate */
    virtual std::string tlsClientCertificate() const = 0;
    /** @brief Client certificate's private key */
    virtual std::string tlsClientCertificatePrivateKey() const = 0;
    /** @brief Client certificate's private key passphrase */
    virtual std::string tlsClientCertificatePrivateKeyPassphrase() const = 0;
    /** @brief Allow TLS connections using self-signed certificates
     *         (Warning : enabling this feature is not recommended in production) */
    virtual bool tlsAllowSelfSignedCertificates() const = 0;
    /** @brief Allow TLS connections using expired certificates
     *         (Warning : enabling this feature is not recommended in production) */
    virtual bool tlsAllowExpiredCertificates() const = 0;
    /** @brief Accept non trusted certificates for TLS connections
     *         (Warning : enabling this feature is not recommended in production) */
    virtual bool tlsAcceptNonTrustedCertificates() const = 0;
    /** @brief Skip server name check in certificates for TLS connections
     *         (Warning : enabling this feature is not recommended in production) */
    virtual bool tlsSkipServerNameCheck() const = 0;

    // Charge point identification

    /** @brief Charge box serial number */
    virtual std::string chargeBoxSerialNumber() const = 0;
    /** @brief Charge point model */
    virtual std::string chargePointModel() const = 0;
    /** @brief Charge point serial number */
    virtual std::string chargePointSerialNumber() const = 0;
    /** @brief Charge point vendor */
    virtual std::string chargePointVendor() const = 0;
    /** @brief Firmware version */
    virtual std::string firmwareVersion() const = 0;
    /** @brief ICCID of the moden's SIM card */
    virtual std::string iccid() const = 0;
    /** @brief IMSI of the moden's SIM card */
    virtual std::string imsi() const = 0;
    /** @brief Main electrical meter serial number */
    virtual std::string meterSerialNumber() const = 0;
    /** @brief Main electrical meter type */
    virtual std::string meterType() const = 0;

    // Charging

    /** @brief Nominal operating voltage (needed for Watt to Amp conversions in smart charging profiles) */
    virtual float operatingVoltage() const = 0;

    // Authent

    /** @brief Maximum number of entries in the authentication cache */
    virtual unsigned int authentCacheMaxEntriesCount() const = 0;

    // Log

    /** @brief Maximum number of entries in the log (0 = no logs in database) */
    virtual unsigned int logMaxEntriesCount() const = 0;

    // Security

    /** @brief Enable internal certificate management : the certificates will be managed by Open OCPP only */
    virtual bool internalCertificateManagementEnabled() const = 0;
    /** @brief Enable security event notification */
    virtual bool securityEventNotificationEnabled() const = 0;
    /** @brief Maximum number of entries in the security log (0 = no security logs in database) */
    virtual unsigned int securityLogMaxEntriesCount() const = 0;
    /** @brief Hash type for certificate request generation : sha256, sha384 or sha512 */
    virtual std::string clientCertificateRequestHashType() const = 0;
    /** @brief Key type for certificate request generation : ec or rsa */
    virtual std::string clientCertificateRequestKeyType() const = 0;
    /** @brief Length in bits of the key for certificate request generation
     *         if rsa has been selected for key type : minimum 2048 */
    virtual unsigned int clientCertificateRequestRsaKeyLength() const = 0;
    /** @brief Name of the elliptic curve for certificate request generation
     *         if ec has been selected for key type : prime256v1, secp256k1, secp384r1, secp521r1,
     *         brainpoolP256t1, brainpoolP384t1 or brainpoolP512t1 */
    virtual std::string clientCertificateRequestEcCurve() const = 0;
    /** @brief Country for the subject field of certificate request generation (can be left empty) */
    virtual std::string clientCertificateRequestSubjectCountry() const = 0;
    /** @brief State for the subject field of certificate request generation (can be left empty) */
    virtual std::string clientCertificateRequestSubjectState() const = 0;
    /** @brief Location for the subject field of certificate request generation (can be left empty) */
    virtual std::string clientCertificateRequestSubjectLocation() const = 0;
    /** @brief Organzation unit for the subject field of certificate request generation (can be left empty) */
    virtual std::string clientCertificateRequestSubjectOrganizationUnit() const = 0;
    /** @brief Email for the subject field of certificate request generation (can be left empty) */
    virtual std::string clientCertificateRequestSubjectEmail() const = 0;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_ICHARGEPOINTCONFIG_H
