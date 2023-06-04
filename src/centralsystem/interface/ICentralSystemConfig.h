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

#ifndef OPENOCPP_ICENTRALSYSTEMCONFIG_H
#define OPENOCPP_ICENTRALSYSTEMCONFIG_H

#include <chrono>
#include <string>

namespace ocpp
{
namespace config
{

/** @brief Interface to retrieve stack internal configuration for the Central System role */
class ICentralSystemConfig
{
  public:
    /** @brief Destructor */
    virtual ~ICentralSystemConfig() { }

    // Paths

    /** @brief Path to the database to store persistent data */
    virtual std::string databasePath() const = 0;
    /** @brief Path to the JSON schemas to validate the messages */
    virtual std::string jsonSchemasPath() const = 0;

    // Communication parameters

    /** @brief Listen URL */
    virtual std::string listenUrl() const = 0;
    /** @brief Call request timeout */
    virtual std::chrono::milliseconds callRequestTimeout() const = 0;
    /** @brief Websocket PING interval */
    virtual std::chrono::seconds webSocketPingInterval() const = 0;
    /** @brief Boot notification retry interval */
    virtual std::chrono::seconds bootNotificationRetryInterval() const = 0;
    /** @brief Heartbeat interval */
    virtual std::chrono::seconds heartbeatInterval() const = 0;
    /** @brief Enable HTTP basic authentication */
    virtual bool httpBasicAuthent() const = 0;
    /** @brief Cipher list to use for TLSv1.2 connections */
    virtual std::string tlsv12CipherList() const = 0;
    /** @brief Cipher list to use for TLSv1.3 connections */
    virtual std::string tlsv13CipherList() const = 0;
    /** @brief ECDH curve to use for TLS connections */
    virtual std::string tlsEcdhCurve() const = 0;
    /** @brief Server certificate */
    virtual std::string tlsServerCertificate() const = 0;
    /** @brief Server certificate's private key */
    virtual std::string tlsServerCertificatePrivateKey() const = 0;
    /** @brief Server certificate's private key passphrase */
    virtual std::string tlsServerCertificatePrivateKeyPassphrase() const = 0;
    /** @brief Certification Authority signing chain for the server certificate */
    virtual std::string tlsServerCertificateCa() const = 0;
    /** @brief Enable client authentication using certificate */
    virtual bool tlsClientCertificateAuthent() const = 0;

    // Log

    /** @brief Maximum number of entries in the log (0 = no logs in database) */
    virtual unsigned int logMaxEntriesCount() const = 0;

    // Behavior

    /** @brief Size of the thread pool to handle incoming requests from the Charge Points */
    virtual unsigned int incomingRequestsFromCpThreadPoolSize() const = 0;

    // ISO 15118 PnC extensions

    /** @brief If this variable set to true, then the Central System supports ISO 15118 plug and charge messages via the DataTransfer mechanism as
               described in this application note. */
    virtual bool iso15118PnCEnabled() const = 0;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_ICENTRALSYSTEMCONFIG_H
