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

#ifndef OPENOCPP_OCPP20_ICHARGEPOINTCONFIG20_H
#define OPENOCPP_OCPP20_ICHARGEPOINTCONFIG20_H

#include <chrono>
#include <string>

namespace ocpp
{
namespace config
{

/** @brief Interface to retrieve stack internal configuration for the Charge Point role */
class IChargePointConfig20
{
  public:
    /** @brief Destructor */
    virtual ~IChargePointConfig20() { }

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
    /** @brief Websocket PING interval */
    virtual std::chrono::seconds webSocketPingInterval() const = 0;
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

    // Log

    /** @brief Maximum number of entries in the log (0 = no logs in database) */
    virtual unsigned int logMaxEntriesCount() const = 0;

    // Security

    /** @brief Security profile to use for connection with the central system [0-3] */
    virtual unsigned int securityProfile() const = 0;
    /** @brief The basic authentication password is used for HTTP Basic Authentication. The password SHALL be a randomly
               chosen passwordString with a sufficiently high entropy, consisting of minimum 16 and maximum 40 characters
               (alpha-numeric characters and the special characters allowed by passwordString). The password SHALL be sent
               as a UTF-8 encoded string (NOT encoded into octet string or base64). This configuration variable is write-only, so
               that it cannot be accidentally stored in plaintext by the CSMS when it reads out all configuration variables.
               This configuration variable is required unless only "security profile 3 - TLS with client side certificates" is
               implemented. */
    virtual std::string basicAuthPassword() const = 0;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_OCPP20_ICHARGEPOINTCONFIG20_H
