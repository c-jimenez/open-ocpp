/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CENTRALSYSTEMCONFIG_H
#define CENTRALSYSTEMCONFIG_H

#include "ICentralSystemConfig.h"
#include "IniFile.h"

/** @brief Section name for the parameters */
static const std::string STACK_PARAMS = "CentralSystem";

/** @brief Charge Point stack internal configuration */
class CentralSystemConfig : public ocpp::config::ICentralSystemConfig
{
  public:
    /** @brief Constructor */
    CentralSystemConfig(ocpp::helpers::IniFile& config) : m_config(config) { }

    // Paths

    /** @brief Path to the database to store persistent data */
    std::string databasePath() const override { return getString("DatabasePath"); }
    /** @brief Path to the JSON schemas to validate the messages */
    std::string jsonSchemasPath() const override { return getString("JsonSchemasPath"); }

    // Communication parameters

    /** @brief Listen URL */
    std::string listenUrl() const override { return getString("ListenUrl"); }
    /** @brief Call request timeout */
    std::chrono::milliseconds callRequestTimeout() const override { return get<std::chrono::milliseconds>("CallRequestTimeout"); }
    /** @brief Websocket PING interval */
    std::chrono::seconds webSocketPingInterval() const override { return get<std::chrono::seconds>("WebSocketPingInterval"); }
    /** @brief Boot notification retry interval */
    std::chrono::seconds bootNotificationRetryInterval() const override
    {
        return get<std::chrono::seconds>("BootNotificationRetryInterval");
    }
    /** @brief Heartbeat interval */
    std::chrono::seconds heartbeatInterval() const override { return get<std::chrono::seconds>("HeartbeatInterval"); }
    /** @brief Enable HTTP basic authentication */
    bool httpBasicAuthent() const override { return getBool("HttpBasicAuthent"); }
    /** @brief Cipher list to use for TLSv1.2 connections */
    std::string tlsv12CipherList() const override { return getString("Tlsv12CipherList"); }
    /** @brief Cipher list to use for TLSv1.3 connections */
    std::string tlsv13CipherList() const override { return getString("Tlsv13CipherList"); }
    /** @brief ECDH curve to use for TLS connections */
    std::string tlsEcdhCurve() const override { return getString("TlsEcdhCurve"); }
    /** @brief Server certificate */
    std::string tlsServerCertificate() const override { return getString("TlsServerCertificate"); }
    /** @brief Server certificate's private key */
    std::string tlsServerCertificatePrivateKey() const override { return getString("TlsServerCertificatePrivateKey"); }
    /** @brief Server certificate's private key passphrase */
    std::string tlsServerCertificatePrivateKeyPassphrase() const override { return getString("TlsServerCertificatePrivateKeyPassphrase"); }
    /** @brief Certification Authority signing chain for the server certificate */
    std::string tlsServerCertificateCa() const override { return getString("TlsServerCertificateCa"); }
    /** @brief Enable client authentication using certificate */
    bool tlsClientCertificateAuthent() const override { return getBool("TlsClientCertificateAuthent"); }

    // Logs

    /** @brief Maximum number of entries in the log (0 = no logs in database) */
    unsigned int logMaxEntriesCount() const override { return get<unsigned int>("LogMaxEntriesCount"); }

    // Behavior

    /** @brief Size of the thread pool to handle incoming requests from the Charge Points */
    unsigned int incomingRequestsFromCpThreadPoolSize() const override
    {
        return get<unsigned int>("IncomingRequestsFromCpThreadPoolSize");
    };

    // ISO 15118 PnC extensions

    /** @brief If this variable set to true, then the Central System supports ISO 15118 plug and charge messages via the DataTransfer mechanism as
               described in this application note. */
    bool iso15118PnCEnabled() const override { return getBool("Iso15118PnCEnabled"); }

  private:
    /** @brief Configuration file */
    ocpp::helpers::IniFile& m_config;

    /** @brief Get a boolean parameter */
    bool getBool(const std::string& param) const { return m_config.get(STACK_PARAMS, param).toBool(); }
    /** @brief Get a floating point parameter */
    double getFloat(const std::string& param) const { return m_config.get(STACK_PARAMS, param).toFloat(); }
    /** @brief Get a string parameter */
    std::string getString(const std::string& param) const { return m_config.get(STACK_PARAMS, param); }
    /** @brief Get a value which can be created from an unsigned integer */
    template <typename T>
    T get(const std::string& param) const
    {
        return T(m_config.get(STACK_PARAMS, param).toUInt());
    }
};

#endif // CENTRALSYSTEMCONFIG_H
