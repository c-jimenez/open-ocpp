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

#ifndef OCPPCONFIG_H
#define OCPPCONFIG_H

#include "IOcppConfig.h"
#include "IniFile.h"

/** @brief Section name for the parameters */
static const std::string OCPP_PARAMS = "Ocpp";

/** @brief Interface to retrieve standard OCPP configuration */
class OcppConfig : public ocpp::config::IOcppConfig
{
  public:
    /** @brief Constructor */
    OcppConfig(ocpp::helpers::IniFile& config);

    ///
    /// Generic getter
    ///

    /** @copydoc void IOcppConfig::getConfiguration(const std::vector<ocpp::types::CiStringType<50u>>&,
     *                                              std::vector<ocpp::types::ocpp16::KeyValue>&,
     *                                              std::vector<ocpp::types::CiStringType<50u>>&);
     */
    void getConfiguration(const std::vector<ocpp::types::CiStringType<50u>>& keys,
                          std::vector<ocpp::types::ocpp16::KeyValue>&        values,
                          std::vector<ocpp::types::CiStringType<50u>>&       unknown_values) override;

    ///
    /// Generic setter
    ///

    /** @copydoc ConfigurationStatus IOcppConfig::setConfiguration(const std::string&, const std::string&) */
    ocpp::types::ocpp16::ConfigurationStatus setConfiguration(const std::string& key, const std::string& value) override;

    //
    // Specific getters
    //

    // Core profile

    /** @brief If this key exists, the Charge Point supports Unknown Offline Authorization. If this key reports a value of true, Unknown Offline
               Authorization is enabled. */
    bool allowOfflineTxForUnknownId() const override { return getBool("AllowOfflineTxForUnknownId"); }
    /** @brief If this key exists, the Charge Point supports an Authorization Cache. If this key reports a value of true, the Authorization Cache
               is enabled. */
    bool authorizationCacheEnabled() const override { return getBool("AuthorizationCacheEnabled"); }
    /** @brief Whether a remote request to start a transaction in the form of a RemoteStartTransaction.req message should be authorized
               beforehand like a local action to start a transaction. */
    bool authorizeRemoteTxRequests() const override { return getBool("AuthorizeRemoteTxRequests"); }
    /** @brief Number of times to blink Charge Point lighting when signalling */
    unsigned int blinkRepeat() const override { return get<unsigned int>("BlinkRepeat"); }
    /** @brief Size (in seconds) of the clock-aligned data interval. This is the size (in seconds) of the set of evenly spaced aggregation intervals
               per day, starting at 00:00:00 (midnight). For example, a value of 900 (15 minutes) indicates that every day should be broken into
               96 15-minute intervals.
               When clock aligned data is being transmitted, the interval in question is identified by the start time and (optional) duration
               interval value, represented according to the ISO8601 standard. All "per-period" data (e.g. energy readings) should be
               accumulated (for "flow" type measurands such as energy), or averaged (for other values) across the entire interval (or partial
               interval, at the beginning or end of a Transaction), and transmitted (if so enabled) at the end of each interval, bearing the
               interval start time timestamp.
               A value of "0" (numeric zero), by convention, is to be interpreted to mean that no clock-aligned data should be transmitted. */
    std::chrono::seconds clockAlignedDataInterval() const override { return get<std::chrono::seconds>("ClockAlignedDataInterval"); }
    /** @brief Interval *from beginning of status: 'Preparing' until incipient Transaction is automatically canceled, due to failure of EV driver to
               (correctly) insert the charging cable connector(s) into the appropriate socket(s). The Charge Point SHALL go back to the original
               state, probably: 'Available'. */
    std::chrono::seconds connectionTimeOut() const override { return get<std::chrono::seconds>("ConnectionTimeOut"); }
    /** @brief The phase rotation per connector in respect to the connector’s electrical meter (or if absent, the grid connection). Possible
               values per connector are:
               NotApplicable (for Single phase or DC Charge Points)
               Unknown (not (yet) known)
               RST (Standard Reference Phasing)
               RTS (Reversed Reference Phasing)
               SRT (Reversed 240 degree rotation)
               STR (Standard 120 degree rotation)
               TRS (Standard 240 degree rotation)
               TSR (Reversed 120 degree rotation)
               R can be identified as phase 1 (L1), S as phase 2 (L2), T as phase 3 (L3).
               If known, the Charge Point MAY also report the phase rotation between the grid connection and the main energymeter by
               using index number Zero (0).
               Values are reported in CSL, formatted: 0.RST, 1.RST, 2.RTS */
    std::string connectorPhaseRotation() const override { return getString("ConnectorPhaseRotation"); }
    /** @brief Maximum number of items in a ConnectorPhaseRotation Configuration Key. */
    unsigned int connectorPhaseRotationMaxLength() const override { return get<unsigned int>("ConnectorPhaseRotationMaxLength"); }
    /** @brief Maximum number of requested configuration keys in a GetConfiguration.req PDU. */
    unsigned int getConfigurationMaxKeys() const override { return get<unsigned int>("GetConfigurationMaxKeys"); }
    /** @brief Interval of inactivity (no OCPP exchanges) with central system after which the Charge Point should send a Heartbeat.req PDU */
    std::chrono::seconds heartbeatInterval() const override { return get<std::chrono::seconds>("HeartbeatInterval"); }
    /** @brief Percentage of maximum intensity at which to illuminate Charge Point lighting */
    unsigned int lightIntensity() const override { return get<unsigned int>("LightIntensity"); }
    /** @brief Whether the Charge Point, when offline, will start a transaction for locally-authorized identifiers. */
    bool localAuthorizeOffline() const override { return getBool("LocalAuthorizeOffline"); }
    /** @brief Whether the Charge Point, when online, will start a transaction for locally-authorized identifiers without waiting for or
               requesting an Authorize.conf from the Central System */
    bool localPreAuthorize() const override { return getBool("LocalPreAuthorize"); }
    /** @brief Maximum energy in Wh delivered when an identifier is invalidated by the Central System after start of a transaction. */
    unsigned int maxEnergyOnInvalidId() const override { return get<unsigned int>("MaxEnergyOnInvalidId"); }
    /** @brief Clock-aligned measurand(s) to be included in a MeterValues.req PDU, every ClockAlignedDataInterval seconds */
    std::string meterValuesAlignedData() const override { return getString("MeterValuesAlignedData"); }
    /** @brief Maximum number of items in a MeterValuesAlignedData Configuration Key. */
    unsigned int meterValuesAlignedDataMaxLength() const override { return get<unsigned int>("MeterValuesAlignedDataMaxLength"); }
    /** @brief Sampled measurands to be included in a MeterValues.req PDU, every MeterValueSampleInterval seconds. Where
               applicable, the Measurand is combined with the optional phase; for instance: Voltage.L1
               Default: "Energy.Active.Import.Register" */
    std::string meterValuesSampledData() const override { return getString("MeterValuesSampledData"); }
    /** @brief Maximum number of items in a MeterValuesSampledData Configuration Key. */
    unsigned int meterValuesSampledDataMaxLength() const override { return get<unsigned int>("MeterValuesSampledDataMaxLength"); }
    /** @brief Interval between sampling of metering (or other) data, intended to be transmitted by "MeterValues" PDUs. For charging
               session data (ConnectorId>0), samples are acquired and transmitted periodically at this interval from the start of the charging
               transaction.
               A value of "0" (numeric zero), by convention, is to be interpreted to mean that no sampled data should be transmitted. */
    std::chrono::seconds meterValueSampleInterval() const override { return get<std::chrono::seconds>("MeterValueSampleInterval"); }
    /** @brief The minimum duration that a Charge Point or Connector status is stable before a StatusNotification.req PDU is sent to the
               Central System. */
    std::chrono::seconds minimumStatusDuration() const override { return get<std::chrono::seconds>("MinimumStatusDuration"); }
    /** @brief The number of physical charging connectors of this Charge Point. */
    unsigned int numberOfConnectors() const override { return get<unsigned int>("NumberOfConnectors"); }
    /** @brief Number of times to retry an unsuccessful reset of the Charge Point. */
    unsigned int resetRetries() const override { return get<unsigned int>("ResetRetries"); }
    /** @brief When set to true, the Charge Point SHALL administratively stop the transaction when the cable is unplugged from the EV. */
    bool stopTransactionOnEVSideDisconnect() const override { return getBool("StopTransactionOnEVSideDisconnect"); }
    /** @brief whether the Charge Point will stop an ongoing transaction when it receives a non- Accepted authorization status in a
               StartTransaction.conf for this transaction */
    bool stopTransactionOnInvalidId() const override { return getBool("StopTransactionOnInvalidId"); }
    /** @brief Clock-aligned periodic measurand(s) to be included in the TransactionData element of StopTransaction.req MeterValues.req
               PDU for every ClockAlignedDataInterval of the Transaction */
    std::string stopTxnAlignedData() const override { return getString("StopTxnAlignedData"); }
    /** @brief Maximum number of items in a StopTxnAlignedData Configuration Key. */
    unsigned int stopTxnAlignedDataMaxLength() const override { return get<unsigned int>("StopTxnAlignedDataMaxLength"); }
    /** @brief Sampled measurands to be included in the TransactionData element of StopTransaction.req PDU, every
               MeterValueSampleInterval seconds from the start of the charging session */
    std::string stopTxnSampledData() const override { return getString("StopTxnSampledData"); }
    /** @brief Maximum number of items in a StopTxnSampledData Configuration Key.*/
    unsigned int stopTxnSampledDataMaxLength() const override { return get<unsigned int>("StopTxnSampledDataMaxLength"); }
    /** @brief A list of supported Feature Profiles. Possible profile identifiers: Core, FirmwareManagement, LocalAuthListManagement,
               Reservation, SmartCharging and RemoteTrigger. */
    std::string supportedFeatureProfiles() const override { return getString("SupportedFeatureProfiles"); }
    /** @brief Maximum number of items in a SupportedFeatureProfiles Configuration Key. */
    unsigned int supportedFeatureProfilesMaxLength() const override { return get<unsigned int>("SupportedFeatureProfilesMaxLength"); }
    /** @brief How often the Charge Point should try to submit a transaction-related message when the Central System fails to process it. */
    unsigned int transactionMessageAttempts() const override { return get<unsigned int>("TransactionMessageAttempts"); }
    /** @brief How long the Charge Point should wait before resubmitting a transaction-related message that the Central System failed to
               process. */
    std::chrono::seconds transactionMessageRetryInterval() const override
    {
        return get<std::chrono::seconds>("TransactionMessageRetryInterval");
    }
    /** @brief When set to true, the Charge Point SHALL unlock the cable on Charge Point side when the cable is unplugged at the EV */
    bool unlockConnectorOnEVSideDisconnect() const override { return getBool("UnlockConnectorOnEVSideDisconnect"); }
    /** @brief Only relevant for websocket implementations. 0 disables client side websocket Ping/Pong. In this case there is either no
               ping/pong or the server initiates the ping and client responds with Pong. Positive values are interpreted as number of seconds
               between pings. Negative values are not allowed. ChangeConfiguration is expected to return a REJECTED result. */
    std::chrono::seconds webSocketPingInterval() const override { return get<std::chrono::seconds>("WebSocketPingInterval"); }

    // Local Auth List Management Profile

    /** @brief whether the Local Authorization List is enabled */
    bool localAuthListEnabled() const override { return getBool("LocalAuthListEnabled"); }
    /** @brief Maximum number of identifications that can be stored in the Local Authorization List */
    unsigned int localAuthListMaxLength() const override { return get<unsigned int>("LocalAuthListMaxLength"); }
    /** @brief Maximum number of identifications that can be send in a single SendLocalList.req */
    unsigned int sendLocalListMaxLength() const override { return get<unsigned int>("SendLocalListMaxLength"); }

    // Reservation Profile

    /** @brief If this configuration key is present and set to true: Charge Point support reservations on connector 0. */
    bool reserveConnectorZeroSupported() const override { return getBool("ReserveConnectorZeroSupported"); }

    // Smart Charging Profile

    /** @brief Max StackLevel of a ChargingProfile. The number defined also indicates the max allowed number of installed charging
               schedules per Charging Profile Purposes. */
    unsigned int chargeProfileMaxStackLevel() const override { return get<unsigned int>("ChargeProfileMaxStackLevel"); }
    /** @brief A list of supported quantities for use in a ChargingSchedule. Allowed values: 'Current' and 'Power' */
    std::string chargingScheduleAllowedChargingRateUnit() const override { return getString("ChargingScheduleAllowedChargingRateUnit"); }
    /** @brief Maximum number of periods that may be defined per ChargingSchedule. */
    unsigned int chargingScheduleMaxPeriods() const override { return get<unsigned int>("ChargingScheduleMaxPeriods"); }
    /** @brief If defined and true, this Charge Point support switching from 3 to 1 phase during a Transaction. */
    bool connectorSwitch3to1PhaseSupported() const override { return getBool("ConnectorSwitch3to1PhaseSupported"); }
    /** @brief Maximum number of Charging profiles installed at a time */
    unsigned int maxChargingProfilesInstalled() const override { return get<unsigned int>("MaxChargingProfilesInstalled"); }

    //
    // Specific setters
    //

    /** @brief Interval of inactivity (no OCPP exchanges) with central system after which the Charge Point should send a Heartbeat.req PDU */
    void heartbeatInterval(std::chrono::seconds interval) override
    {
        m_config.set(OCPP_PARAMS, "HeartbeatInterval", ocpp::helpers::IniFile::Value(static_cast<unsigned int>(interval.count())));
    }

    //
    // Security extensions
    //

    /** @brief When set to true, only one certificate (plus a temporarily fallback certificate) of certificateType CentralSystemRootCertificate is
               allowed to be installed at a time. When installing a new Central System Root certificate, the new certificate SHALL replace the
               old one AND the new Central System Root Certificate MUST be signed by the old Central System Root Certificate it is replacing.
               This configuration key is required unless only "security profile 1 - Unsecured Transport with Basic Authentication" is
               implemented. Please note that security profile 1 SHOULD only be used in trusted networks.

               Note: When using this additional security mechanism please be aware that the Charge Point needs to perform a full certificate chain
               verification when the new Central System Root certificate is being installed. However, once the old Central System Root certificate is set
               as the fallback certificate, the Charge Point needs to perform a partial certificate chain verification when verifying the server certificate
               during the TLS handshake. Otherwise the verification will fail once the old Central System Root (fallback) certificate is either expired or
               removed. */
    bool additionalRootCertificateCheck() const override { return getBool("AdditionalRootCertificateCheck"); }

    /** @brief The basic authentication password is used for HTTP Basic Authentication, minimal length: 16 bytes.
                It is strongly advised to be randomly generated binary to get maximal entropy. Hexadecimal represented (20 bytes maximum,
                represented as a string of up to 40 hexadecimal digits).
                This configuration key is write-only, so that it cannot be accidentally stored in plaintext by the Central System when it reads out
                all configuration keys.
                This configuration key is required unless only "security profile 3 - TLS with client side certificates" is implemented. */
    std::string authorizationKey() const override { return getString("AuthorizationKey"); }

    /** @brief This configuration key can be used to limit the size of the 'certificateChain' field from the CertificateSigned.req PDU. The value
               of this configuration key has a maximum limit of 10.000 characters. */
    unsigned int certificateSignedMaxChainSize() const override { return get<unsigned int>("CertificateSignedMaxChainSize"); }

    /** @brief Maximum number of Root/CA certificates that can be installed in the Charge Point. */
    unsigned int certificateStoreMaxLength() const override { return get<unsigned int>("CertificateStoreMaxLength"); }

    /** @brief This configuration key contains CPO name (or an organization trusted by the CPO) as used in the Charge Point Certificate. This
               is the CPO name that is to be used in a CSR send via: SignCertificate.req */
    std::string cpoName() const override { return getString("CpoName"); }

    /** @brief This configuration key is used to set the security profile used by the Charge Point.
               The value of this configuration key can only be increased to a higher level, not decreased to a lower level, if the Charge Point
               receives a lower value then currently configured,the Charge Point SHALL Rejected the ChangeConfiguration.req
               Before accepting the new value, the Charge Point SHALL check if all the prerequisites for the new Security Profile are met, if
               not, the Charge Point SHALL Rejected the ChangeConfiguration.req.
               After the security profile was successfully changed, the Charge Point disconnects from the Central System and SHALL
               reconnect using the new configured Security Profile.
               Default, when no security profile is yet configured: 0. */
    unsigned int securityProfile() const override { return get<unsigned int>("SecurityProfile"); }

    /** @brief Comma separated list of supported file transfer protocols for upload AND download
               Allowed values : FTP, FTPS, HTTP, HTTPS, SFTP */
    std::string supportedFileTransferProtocols() const override { return getString("SupportedFileTransferProtocols"); }

    //
    // ISO 15118 PnC extensions
    //

    /** @brief If this variable exists and has the value true, then the Charge Point can provide a contract certificate that it cannot 
               validate to the Central System for validation as part of the Authorize.req */
    bool centralContractValidationAllowed() const override { return getBool("CentralContractValidationAllowed"); }

    /** @brief This configuration key defines how long the Charge Point has to wait (in seconds) before generating another CSR, in the case the
               Central System accepts the SignCertificate.req, but never returns the signed certificate back. This value will be doubled after every
               attempt. The amount of attempts is configured at CertSigningRepeatTimes. If the certificate signing process is slow, this setting
               allows the Central System to tell the Charge Point to allow more time.
               Negative values must be rejected. The value 0 means that the Charge Point does not generate another CSR (leaving it up to the
               Central System to trigger another certificate installation). */
    std::chrono::seconds certSigningWaitMinimum() const override { return get<std::chrono::seconds>("CertSigningWaitMinimum"); }

    /** @brief This configuration key can be used to configure the amount of times the Charge Point SHALL double the previous back-off time,
               starting with the number of seconds configured at CertSigningWaitMinimum, every time the back-off time expires without having
               received the CertificateSigned.req containing the signed certificate based on the CSR generated. When the maximum number of
               increments is reached, the Charge Point SHALL stop resending the SignCertificate.req, until it is requested by the Central System
               using a TriggerMessage.req.
               Negative values must be rejected. The value 0 means that the Charge Point does not double the back-off time. */
    unsigned int certSigningRepeatTimes() const override { return get<unsigned int>("CertSigningRepeatTimes"); }

    /** @brief If this variable is true, then the Charge Point will try to validate a contract certificate when it is offline. */
    bool contractValidationOffline() const override { return getBool("ContractValidationOffline"); }

    /** @brief If this variable set to true, then the Charge Point supports ISO 15118 plug and charge messages via the DataTransfer mechanism as
               described in this application note. */
    bool iso15118PnCEnabled() const override { return getBool("Iso15118PnCEnabled"); }

  private:
    /** @brief Configuration file */
    ocpp::helpers::IniFile& m_config;

    /** @brief Get a boolean parameter */
    bool getBool(const std::string& param) const { return m_config.get(OCPP_PARAMS, param).toBool(); }
    /** @brief Get a string parameter */
    std::string getString(const std::string& param) const { return m_config.get(OCPP_PARAMS, param); }
    /** @brief Get a value which can be created from an unsigned integer */
    template <typename T>
    T get(const std::string& param) const
    {
        return T(m_config.get(OCPP_PARAMS, param).toUInt());
    }
};

#endif // OCPPCONFIG_H
