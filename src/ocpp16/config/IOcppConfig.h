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

#ifndef OPENOCPP_IOCPPCONFIG_H
#define OPENOCPP_IOCPPCONFIG_H

#include "CiStringType.h"
#include "Enums.h"
#include "KeyValue.h"

#include <chrono>
#include <string>
#include <vector>

namespace ocpp
{
namespace config
{

/** @brief Interface to retrieve standard OCPP configuration */
class IOcppConfig
{
  public:
    /** @brief Destructor */
    virtual ~IOcppConfig() { }

    ///
    /// Generic getter
    ///

    /**
     * @brief Retrieve a list of configuration values with their attributes
     * @param keys List of configuration values to retrieve  (if empty, the
     *             whole available parameters list must be returned in the [values] parameter)
     * @param values Value and attributes of known parameters
     * @param unknown_values List of unknown parameters in the [keys] list
     */
    virtual void getConfiguration(const std::vector<ocpp::types::CiStringType<50u>>& keys,
                                  std::vector<ocpp::types::ocpp16::KeyValue>&        values,
                                  std::vector<ocpp::types::CiStringType<50u>>&       unknown_values) = 0;

    /**
     * @brief Set a configuration value
     * @param key Name of the configuration value
     * @param value New value
     * @return Status of the operation (see ConfigurationStatus)
     */
    virtual ocpp::types::ocpp16::ConfigurationStatus setConfiguration(const std::string& key, const std::string& value) = 0;

    //
    // Specific getters
    //

    // Core profile

    /** @brief If this key exists, the Charge Point supports Unknown Offline Authorization. If this key reports a value of true, Unknown Offline
               Authorization is enabled. */
    virtual bool allowOfflineTxForUnknownId() const = 0;
    /** @brief If this key exists, the Charge Point supports an Authorization Cache. If this key reports a value of true, the Authorization Cache
               is enabled. */
    virtual bool authorizationCacheEnabled() const = 0;
    /** @brief Whether a remote request to start a transaction in the form of a RemoteStartTransaction.req message should be authorized
               beforehand like a local action to start a transaction. */
    virtual bool authorizeRemoteTxRequests() const = 0;
    /** @brief Number of times to blink Charge Point lighting when signalling */
    virtual unsigned int blinkRepeat() const = 0;
    /** @brief Size (in seconds) of the clock-aligned data interval. This is the size (in seconds) of the set of evenly spaced aggregation intervals
               per day, starting at 00:00:00 (midnight). For example, a value of 900 (15 minutes) indicates that every day should be broken into
               96 15-minute intervals.
               When clock aligned data is being transmitted, the interval in question is identified by the start time and (optional) duration
               interval value, represented according to the ISO8601 standard. All "per-period" data (e.g. energy readings) should be
               accumulated (for "flow" type measurands such as energy), or averaged (for other values) across the entire interval (or partial
               interval, at the beginning or end of a Transaction), and transmitted (if so enabled) at the end of each interval, bearing the
               interval start time timestamp.
               A value of "0" (numeric zero), by convention, is to be interpreted to mean that no clock-aligned data should be transmitted. */
    virtual std::chrono::seconds clockAlignedDataInterval() const = 0;
    /** @brief Interval *from beginning of status: 'Preparing' until incipient Transaction is automatically canceled, due to failure of EV driver to
               (correctly) insert the charging cable connector(s) into the appropriate socket(s). The Charge Point SHALL go back to the original
               state, probably: 'Available'. */
    virtual std::chrono::seconds connectionTimeOut() const = 0;
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
    virtual std::string connectorPhaseRotation() const = 0;
    /** @brief Maximum number of items in a ConnectorPhaseRotation Configuration Key. */
    virtual unsigned int connectorPhaseRotationMaxLength() const = 0;
    /** @brief Maximum number of requested configuration keys in a GetConfiguration.req PDU. */
    virtual unsigned int getConfigurationMaxKeys() const = 0;
    /** @brief Interval of inactivity (no OCPP exchanges) with central system after which the Charge Point should send a Heartbeat.req PDU */
    virtual std::chrono::seconds heartbeatInterval() const = 0;
    /** @brief Percentage of maximum intensity at which to illuminate Charge Point lighting */
    virtual unsigned int lightIntensity() const = 0;
    /** @brief Whether the Charge Point, when offline, will start a transaction for locally-authorized identifiers. */
    virtual bool localAuthorizeOffline() const = 0;
    /** @brief Whether the Charge Point, when online, will start a transaction for locally-authorized identifiers without waiting for or
               requesting an Authorize.conf from the Central System */
    virtual bool localPreAuthorize() const = 0;
    /** @brief Maximum energy in Wh delivered when an identifier is invalidated by the Central System after start of a transaction. */
    virtual unsigned int maxEnergyOnInvalidId() const = 0;
    /** @brief Clock-aligned measurand(s) to be included in a MeterValues.req PDU, every ClockAlignedDataInterval seconds */
    virtual std::string meterValuesAlignedData() const = 0;
    /** @brief Maximum number of items in a MeterValuesAlignedData Configuration Key. */
    virtual unsigned int meterValuesAlignedDataMaxLength() const = 0;
    /** @brief Sampled measurands to be included in a MeterValues.req PDU, every MeterValueSampleInterval seconds. Where
               applicable, the Measurand is combined with the optional phase; for instance: Voltage.L1
               Default: "Energy.Active.Import.Register" */
    virtual std::string meterValuesSampledData() const = 0;
    /** @brief Maximum number of items in a MeterValuesSampledData Configuration Key. */
    virtual unsigned int meterValuesSampledDataMaxLength() const = 0;
    /** @brief Interval between sampling of metering (or other) data, intended to be transmitted by "MeterValues" PDUs. For charging
               session data (ConnectorId>0), samples are acquired and transmitted periodically at this interval from the start of the charging
               transaction.
               A value of "0" (numeric zero), by convention, is to be interpreted to mean that no sampled data should be transmitted. */
    virtual std::chrono::seconds meterValueSampleInterval() const = 0;
    /** @brief The minimum duration that a Charge Point or Connector status is stable before a StatusNotification.req PDU is sent to the
               Central System. */
    virtual std::chrono::seconds minimumStatusDuration() const = 0;
    /** @brief The number of physical charging connectors of this Charge Point. */
    virtual unsigned int numberOfConnectors() const = 0;
    /** @brief Number of times to retry an unsuccessful reset of the Charge Point. */
    virtual unsigned int resetRetries() const = 0;
    /** @brief When set to true, the Charge Point SHALL administratively stop the transaction when the cable is unplugged from the EV. */
    virtual bool stopTransactionOnEVSideDisconnect() const = 0;
    /** @brief whether the Charge Point will stop an ongoing transaction when it receives a non- Accepted authorization status in a
               StartTransaction.conf for this transaction */
    virtual bool stopTransactionOnInvalidId() const = 0;
    /** @brief Clock-aligned periodic measurand(s) to be included in the TransactionData element of StopTransaction.req MeterValues.req
               PDU for every ClockAlignedDataInterval of the Transaction */
    virtual std::string stopTxnAlignedData() const = 0;
    /** @brief Maximum number of items in a StopTxnAlignedData Configuration Key. */
    virtual unsigned int stopTxnAlignedDataMaxLength() const = 0;
    /** @brief Sampled measurands to be included in the TransactionData element of StopTransaction.req PDU, every
               MeterValueSampleInterval seconds from the start of the charging session */
    virtual std::string stopTxnSampledData() const = 0;
    /** @brief Maximum number of items in a StopTxnSampledData Configuration Key.*/
    virtual unsigned int stopTxnSampledDataMaxLength() const = 0;
    /** @brief A list of supported Feature Profiles. Possible profile identifiers: Core, FirmwareManagement, LocalAuthListManagement,
               Reservation, SmartCharging and RemoteTrigger. */
    virtual std::string supportedFeatureProfiles() const = 0;
    /** @brief Maximum number of items in a SupportedFeatureProfiles Configuration Key. */
    virtual unsigned int supportedFeatureProfilesMaxLength() const = 0;
    /** @brief How often the Charge Point should try to submit a transaction-related message when the Central System fails to process it. */
    virtual unsigned int transactionMessageAttempts() const = 0;
    /** @brief How long the Charge Point should wait before resubmitting a transaction-related message that the Central System failed to
               process. */
    virtual std::chrono::seconds transactionMessageRetryInterval() const = 0;
    /** @brief When set to true, the Charge Point SHALL unlock the cable on Charge Point side when the cable is unplugged at the EV */
    virtual bool unlockConnectorOnEVSideDisconnect() const = 0;
    /** @brief Only relevant for websocket implementations. 0 disables client side websocket Ping/Pong. In this case there is either no
               ping/pong or the server initiates the ping and client responds with Pong. Positive values are interpreted as number of seconds
               between pings. Negative values are not allowed. ChangeConfiguration is expected to return a REJECTED result. */
    virtual std::chrono::seconds webSocketPingInterval() const = 0;

    // Local Auth List Management Profile

    /** @brief whether the Local Authorization List is enabled */
    virtual bool localAuthListEnabled() const = 0;
    /** @brief Maximum number of identifications that can be stored in the Local Authorization List */
    virtual unsigned int localAuthListMaxLength() const = 0;
    /** @brief Maximum number of identifications that can be send in a single SendLocalList.req */
    virtual unsigned int sendLocalListMaxLength() const = 0;

    // Reservation Profile

    /** @brief If this configuration key is present and set to true: Charge Point support reservations on connector 0. */
    virtual bool reserveConnectorZeroSupported() const = 0;

    // Smart Charging Profile

    /** @brief Max StackLevel of a ChargingProfile. The number defined also indicates the max allowed number of installed charging
               schedules per Charging Profile Purposes. */
    virtual unsigned int chargeProfileMaxStackLevel() const = 0;
    /** @brief A list of supported quantities for use in a ChargingSchedule. Allowed values: 'Current' and 'Power' */
    virtual std::string chargingScheduleAllowedChargingRateUnit() const = 0;
    /** @brief Maximum number of periods that may be defined per ChargingSchedule. */
    virtual unsigned int chargingScheduleMaxPeriods() const = 0;
    /** @brief If defined and true, this Charge Point support switching from 3 to 1 phase during a Transaction. */
    virtual bool connectorSwitch3to1PhaseSupported() const = 0;
    /** @brief Maximum number of Charging profiles installed at a time */
    virtual unsigned int maxChargingProfilesInstalled() const = 0;

    //
    // Specific setters
    //

    /** @brief Interval of inactivity (no OCPP exchanges) with central system after which the Charge Point should send a Heartbeat.req PDU */
    virtual void heartbeatInterval(std::chrono::seconds interval) = 0;

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
    virtual bool additionalRootCertificateCheck() const = 0;

    /** @brief The basic authentication password is used for HTTP Basic Authentication, minimal length: 16 bytes.
                It is strongly advised to be randomly generated binary to get maximal entropy. Hexadecimal represented (20 bytes maximum,
                represented as a string of up to 40 hexadecimal digits).
                This configuration key is write-only, so that it cannot be accidentally stored in plaintext by the Central System when it reads out
                all configuration keys.
                This configuration key is required unless only "security profile 3 - TLS with client side certificates" is implemented. */
    virtual std::string authorizationKey() const = 0;

    /** @brief This configuration key can be used to limit the size of the 'certificateChain' field from the CertificateSigned.req PDU. The value
               of this configuration key has a maximum limit of 10.000 characters. */
    virtual unsigned int certificateSignedMaxChainSize() const = 0;

    /** @brief Maximum number of Root/CA certificates that can be installed in the Charge Point. */
    virtual unsigned int certificateStoreMaxLength() const = 0;

    /** @brief This configuration key contains CPO name (or an organization trusted by the CPO) as used in the Charge Point Certificate. This
               is the CPO name that is to be used in a CSR send via: SignCertificate.req */
    virtual std::string cpoName() const = 0;

    /** @brief This configuration key is used to set the security profile used by the Charge Point.
               The value of this configuration key can only be increased to a higher level, not decreased to a lower level, if the Charge Point
               receives a lower value then currently configured,the Charge Point SHALL Rejected the ChangeConfiguration.req
               Before accepting the new value, the Charge Point SHALL check if all the prerequisites for the new Security Profile are met, if
               not, the Charge Point SHALL Rejected the ChangeConfiguration.req.
               After the security profile was successfully changed, the Charge Point disconnects from the Central System and SHALL
               reconnect using the new configured Security Profile.
               Default, when no security profile is yet configured: 0. */
    virtual unsigned int securityProfile() const = 0;

    /** @brief Comma separated list of supported file transfer protocols for upload AND download
               Allowed values : FTP, FTPS, HTTP, HTTPS, SFTP */
    virtual std::string supportedFileTransferProtocols() const = 0;

    //
    // ISO 15118 PnC extensions
    //

    /** @brief If this variable exists and has the value true, then the Charge Point can provide a contract certificate that it cannot
               validate to the Central System for validation as part of the Authorize.req */
    virtual bool centralContractValidationAllowed() const = 0;

    /** @brief This configuration key defines how long the Charge Point has to wait (in seconds) before generating another CSR, in the case the
               Central System accepts the SignCertificate.req, but never returns the signed certificate back. This value will be doubled after every
               attempt. The amount of attempts is configured at CertSigningRepeatTimes. If the certificate signing process is slow, this setting
               allows the Central System to tell the Charge Point to allow more time.
               Negative values must be rejected. The value 0 means that the Charge Point does not generate another CSR (leaving it up to the
               Central System to trigger another certificate installation). */
    virtual std::chrono::seconds certSigningWaitMinimum() const = 0;

    /** @brief This configuration key can be used to configure the amount of times the Charge Point SHALL double the previous back-off time,
               starting with the number of seconds configured at CertSigningWaitMinimum, every time the back-off time expires without having
               received the CertificateSigned.req containing the signed certificate based on the CSR generated. When the maximum number of
               increments is reached, the Charge Point SHALL stop resending the SignCertificate.req, until it is requested by the Central System
               using a TriggerMessage.req.
               Negative values must be rejected. The value 0 means that the Charge Point does not double the back-off time. */
    virtual unsigned int certSigningRepeatTimes() const = 0;

    /** @brief If this variable is true, then the Charge Point will try to validate a contract certificate when it is offline. */
    virtual bool contractValidationOffline() const = 0;

    /** @brief If this variable set to true, then the Charge Point supports ISO 15118 plug and charge messages via the DataTransfer mechanism as
               described in this application note. */
    virtual bool iso15118PnCEnabled() const = 0;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_IOCPPCONFIG_H
