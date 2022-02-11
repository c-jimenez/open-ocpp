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

#include "ICentralSystem.h"
#include "ICentralSystemEventsHandler.h"
#include "IChargePoint.h"
#include "IChargePointEventsHandler.h"

using namespace ocpp;
using namespace ocpp::config;
using namespace ocpp::types;
using namespace ocpp::centralsystem;
using namespace ocpp::chargepoint;

/** @brief Dummy implementation of central system configuration */
class CentralSystemConfig : public ICentralSystemConfig
{
  public:
    /** @brief Destructor */
    virtual ~CentralSystemConfig() { }

    // Paths

    /** @brief Path to the database to store persistent data */
    std::string databasePath() const override { return ""; }
    /** @brief Path to the JSON schemas to validate the messages */
    std::string jsonSchemasPath() const override { return ""; }

    // Communication parameters

    /** @brief Listen URL */
    std::string listenUrl() const override { return "ws://localhost/ocpp"; }
    /** @brief Call request timeout */
    std::chrono::milliseconds callRequestTimeout() const override { return std::chrono::milliseconds(1000); }
    /** @brief Websocket PING interval */
    std::chrono::seconds webSocketPingInterval() const override { return std::chrono::seconds(10); }
    /** @brief Boot notification retry interval */
    std::chrono::seconds bootNotificationRetryInterval() const override { return std::chrono::seconds(100); }
    /** @brief Heartbeat interval */
    std::chrono::seconds heartbeatInterval() const override { return std::chrono::seconds(100); }
    /** @brief Enable HTTP basic authentication */
    bool httpBasicAuthent() const override { return false; }
    /** @brief Cipher list to use for TLSv1.2 connections */
    std::string tlsv12CipherList() const override { return ""; }
    /** @brief Cipher list to use for TLSv1.3 connections */
    std::string tlsv13CipherList() const override { return ""; }
    /** @brief ECDH curve to use for TLS connections */
    std::string tlsEcdhCurve() const override { return ""; }
    /** @brief Server certificate */
    std::string tlsServerCertificate() const override { return ""; }
    /** @brief Server certificate's private key */
    std::string tlsServerCertificatePrivateKey() const override { return ""; }
    /** @brief Server certificate's private key passphrase */
    std::string tlsServerCertificatePrivateKeyPassphrase() const override { return ""; }
    /** @brief Certification Authority signing chain for the server certificate */
    std::string tlsServerCertificateCa() const override { return ""; }
    /** @brief Enable client authentication using certificate */
    bool tlsClientCertificateAuthent() const override { return false; }

    // Log

    /** @brief Maximum number of entries in the log (0 = no logs in database) */
    unsigned int logMaxEntriesCount() const override { return 1000; }
};

/** @brief Dummy implementation of central system event handler */
class CentralSystemEventsHandler : public ICentralSystemEventsHandler
{
  public:
    /** @brief Destructor */
    virtual ~CentralSystemEventsHandler() { }

    /**
     * @brief Called to check the charge point credentials for HTTP basic authentication
     * @param chargepoint_id Charge Point identifier
     * @param password Password
     * @return true if the credentials are valid, false otherwise
     */
    bool checkCredentials(const std::string& chargepoint_id, const std::string& password) override
    {
        (void)chargepoint_id;
        (void)password;
        return true;
    }

    /**
     * @brief Called when a charge point is connected
     * @param chargepoint Charge point connection
     */
    void chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint> chargepoint) override { (void)chargepoint; }
};

/** @brief Dummy implementation of charge point configuration */
class ChargePointConfig : public IChargePointConfig
{
  public:
    /** @brief Destructor */
    virtual ~ChargePointConfig() { }

    // Paths

    /** @brief Path to the database to store persistent data */
    std::string databasePath() const override { return ""; }
    /** @brief Path to the JSON schemas to validate the messages */
    std::string jsonSchemasPath() const override { return ""; }

    // Communication parameters

    /** @brief Connection URL */
    std::string connexionUrl() const override { return "ws://localhost/ocpp"; }
    /** @brief Charge point identifier */
    std::string chargePointIdentifier() const override { return "CP"; }
    /** @brief Connection timeout */
    std::chrono::milliseconds connectionTimeout() const override { return std::chrono::milliseconds(1000); }
    /** @brief Retry interval */
    std::chrono::milliseconds retryInterval() const override { return std::chrono::milliseconds(1000); }
    /** @brief Call request timeout */
    std::chrono::milliseconds callRequestTimeout() const override { return std::chrono::milliseconds(1000); }
    /** @brief Cipher list to use for TLSv1.2 connections */
    std::string tlsv12CipherList() const override { return ""; }
    /** @brief Cipher list to use for TLSv1.3 connections */
    std::string tlsv13CipherList() const override { return ""; }
    /** @brief Certification Authority signing chain for the server certificate */
    std::string tlsServerCertificateCa() const override { return ""; }
    /** @brief Client certificate */
    std::string tlsClientCertificate() const override { return ""; }
    /** @brief Client certificate's private key */
    std::string tlsClientCertificatePrivateKey() const override { return ""; }
    /** @brief Client certificate's private key passphrase */
    std::string tlsClientCertificatePrivateKeyPassphrase() const override { return ""; }
    /** @brief Allow TLS connections using self-signed certificates
     *         (Warning : enabling this feature is not recommended in production) */
    bool tlsAllowSelfSignedCertificates() const override { return false; }
    /** @brief Allow TLS connections using expired certificates
     *         (Warning : enabling this feature is not recommended in production) */
    bool tlsAllowExpiredCertificates() const override { return false; }
    /** @brief Accept non trusted certificates for TLS connections
     *         (Warning : enabling this feature is not recommended in production) */
    bool tlsAcceptNonTrustedCertificates() const override { return false; }
    /** @brief Skip server name check in certificates for TLS connections
     *         (Warning : enabling this feature is not recommended in production) */
    bool tlsSkipServerNameCheck() const override { return false; }

    // Charge point identification

    /** @brief Charge box serial number */
    std::string chargeBoxSerialNumber() const override { return ""; }
    /** @brief Charge point model */
    std::string chargePointModel() const override { return ""; }
    /** @brief Charge point serial number */
    std::string chargePointSerialNumber() const override { return ""; }
    /** @brief Charge point vendor */
    std::string chargePointVendor() const override { return ""; }
    /** @brief Firmware version */
    std::string firmwareVersion() const override { return ""; }
    /** @brief ICCID of the moden's SIM card */
    std::string iccid() const override { return ""; }
    /** @brief IMSI of the moden's SIM card */
    std::string imsi() const override { return ""; }
    /** @brief Main electrical meter serial number */
    std::string meterSerialNumber() const override { return ""; }
    /** @brief Main electrical meter type */
    std::string meterType() const override { return ""; }

    // Charging

    /** @brief Nominal operating voltage (needed for Watt to Amp conversions in smart charging profiles) */
    float operatingVoltage() const override { return 230.f; }

    // Authent

    /** @brief Maximum number of entries in the authentication cache */
    unsigned int authentCacheMaxEntriesCount() const override { return 100u; }

    // Log

    /** @brief Maximum number of entries in the log (0 = no logs in database) */
    unsigned int logMaxEntriesCount() const override { return 100u; }

    // Security

    /** @brief Enable internal certificate management : the certificates will be managed by Open OCPP only */
    bool internalCertificateManagementEnabled() const override { return true; }
    /** @brief Enable security event notification */
    bool securityEventNotificationEnabled() const override { return true; }
    /** @brief Maximum number of entries in the security log (0 = no security logs in database) */
    unsigned int securityLogMaxEntriesCount() const override { return 100u; }
    /** @brief Hash type for certificate request generation : sha256, sha384 or sha512 */
    std::string clientCertificateRequestHashType() const override { return ""; }
    /** @brief Key type for certificate request generation : ec or rsa */
    std::string clientCertificateRequestKeyType() const override { return ""; }
    /** @brief Length in bits of the key for certificate request generation 
     *         if rsa has been selected for key type : minimum 2048 */
    unsigned int clientCertificateRequestRsaKeyLength() const override { return 4096u; }
    /** @brief Name of the elliptic curve for certificate request generation 
     *         if ec has been selected for key type : prime256v1, secp256k1, secp384r1, secp521r1, 
     *         brainpoolP256t1, brainpoolP384t1 or brainpoolP512t1 */
    std::string clientCertificateRequestEcCurve() const override { return ""; }
    /** @brief Country for the subject field of certificate request generation (can be left empty) */
    std::string clientCertificateRequestSubjectCountry() const override { return ""; }
    /** @brief State for the subject field of certificate request generation (can be left empty) */
    std::string clientCertificateRequestSubjectState() const override { return ""; }
    /** @brief Location for the subject field of certificate request generation (can be left empty) */
    std::string clientCertificateRequestSubjectLocation() const override { return ""; }
    /** @brief Organzation unit for the subject field of certificate request generation (can be left empty) */
    std::string clientCertificateRequestSubjectOrganizationUnit() const override { return ""; }
    /** @brief Email for the subject field of certificate request generation (can be left empty) */
    std::string clientCertificateRequestSubjectEmail() const override { return ""; }
};

/** @brief Dummy implementation of OCPP configuration */
class OcppConfig : public IOcppConfig
{
  public:
    /** @brief Destructor */
    virtual ~OcppConfig() { }

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
    void getConfiguration(const std::vector<ocpp::types::CiStringType<50u>>& keys,
                          std::vector<ocpp::types::KeyValue>&                values,
                          std::vector<ocpp::types::CiStringType<50u>>&       unknown_values) override
    {
        (void)keys;
        (void)values;
        (void)unknown_values;
    }

    /**
     * @brief Set a configuration value
     * @param key Name of the configuration value
     * @param value New value
     * @return Status of the operation (see ConfigurationStatus)
     */
    ocpp::types::ConfigurationStatus setConfiguration(const std::string& key, const std::string& value) override
    {
        (void)key;
        (void)value;
        return ConfigurationStatus::Rejected;
    }

    //
    // Specific getters
    //

    // Core profile

    /** @brief If this key exists, the Charge Point supports Unknown Offline Authorization. If this key reports a value of true, Unknown Offline
               Authorization is enabled. */
    bool allowOfflineTxForUnknownId() const override { return true; }
    /** @brief If this key exists, the Charge Point supports an Authorization Cache. If this key reports a value of true, the Authorization Cache
               is enabled. */
    bool authorizationCacheEnabled() const override { return true; }
    /** @brief Whether a remote request to start a transaction in the form of a RemoteStartTransaction.req message should be authorized
               beforehand like a local action to start a transaction. */
    bool authorizeRemoteTxRequests() const override { return true; }
    /** @brief Number of times to blink Charge Point lighting when signalling */
    unsigned int blinkRepeat() const override { return 0; }
    /** @brief Size (in seconds) of the clock-aligned data interval. This is the size (in seconds) of the set of evenly spaced aggregation intervals
               per day, starting at 00:00:00 (midnight). For example, a value of 900 (15 minutes) indicates that every day should be broken into
               96 15-minute intervals.
               When clock aligned data is being transmitted, the interval in question is identified by the start time and (optional) duration
               interval value, represented according to the ISO8601 standard. All "per-period" data (e.g. energy readings) should be
               accumulated (for "flow" type measurands such as energy), or averaged (for other values) across the entire interval (or partial
               interval, at the beginning or end of a Transaction), and transmitted (if so enabled) at the end of each interval, bearing the
               interval start time timestamp.
               A value of "0" (numeric zero), by convention, is to be interpreted to mean that no clock-aligned data should be transmitted. */
    std::chrono::seconds clockAlignedDataInterval() const override { return std::chrono::seconds(1); }
    /** @brief Interval *from beginning of status: 'Preparing' until incipient Transaction is automatically canceled, due to failure of EV driver to
               (correctly) insert the charging cable connector(s) into the appropriate socket(s). The Charge Point SHALL go back to the original
               state, probably: 'Available'. */
    std::chrono::seconds connectionTimeOut() const override { return std::chrono::seconds(1); }
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
    std::string connectorPhaseRotation() const override { return ""; }
    /** @brief Maximum number of items in a ConnectorPhaseRotation Configuration Key. */
    unsigned int connectorPhaseRotationMaxLength() const override { return 1u; }
    /** @brief Maximum number of requested configuration keys in a GetConfiguration.req PDU. */
    unsigned int getConfigurationMaxKeys() const override { return 1u; }
    /** @brief Interval of inactivity (no OCPP exchanges) with central system after which the Charge Point should send a Heartbeat.req PDU */
    std::chrono::seconds heartbeatInterval() const override { return std::chrono::seconds(1); }
    /** @brief Percentage of maximum intensity at which to illuminate Charge Point lighting */
    unsigned int lightIntensity() const override { return 1u; }
    /** @brief Whether the Charge Point, when offline, will start a transaction for locally-authorized identifiers. */
    bool localAuthorizeOffline() const override { return true; }
    /** @brief Whether the Charge Point, when online, will start a transaction for locally-authorized identifiers without waiting for or
               requesting an Authorize.conf from the Central System */
    bool localPreAuthorize() const override { return true; }
    /** @brief Maximum energy in Wh delivered when an identifier is invalidated by the Central System after start of a transaction. */
    unsigned int maxEnergyOnInvalidId() const override { return 0; }
    /** @brief Clock-aligned measurand(s) to be included in a MeterValues.req PDU, every ClockAlignedDataInterval seconds */
    std::string meterValuesAlignedData() const override { return ""; }
    /** @brief Maximum number of items in a MeterValuesAlignedData Configuration Key. */
    unsigned int meterValuesAlignedDataMaxLength() const override { return 0; }
    /** @brief Sampled measurands to be included in a MeterValues.req PDU, every MeterValueSampleInterval seconds. Where
               applicable, the Measurand is combined with the optional phase; for instance: Voltage.L1
               Default: "Energy.Active.Import.Register" */
    std::string meterValuesSampledData() const override { return ""; }
    /** @brief Maximum number of items in a MeterValuesSampledData Configuration Key. */
    unsigned int meterValuesSampledDataMaxLength() const override { return 0; }
    /** @brief Interval between sampling of metering (or other) data, intended to be transmitted by "MeterValues" PDUs. For charging
               session data (ConnectorId>0), samples are acquired and transmitted periodically at this interval from the start of the charging
               transaction.
               A value of "0" (numeric zero), by convention, is to be interpreted to mean that no sampled data should be transmitted. */
    std::chrono::seconds meterValueSampleInterval() const override { return std::chrono::seconds(1); }
    /** @brief The minimum duration that a Charge Point or Connector status is stable before a StatusNotification.req PDU is sent to the
               Central System. */
    std::chrono::seconds minimumStatusDuration() const override { return std::chrono::seconds(1); }
    /** @brief The number of physical charging connectors of this Charge Point. */
    unsigned int numberOfConnectors() const override { return 1u; }
    /** @brief Number of times to retry an unsuccessful reset of the Charge Point. */
    unsigned int resetRetries() const override { return 2u; }
    /** @brief When set to true, the Charge Point SHALL administratively stop the transaction when the cable is unplugged from the EV. */
    bool stopTransactionOnEVSideDisconnect() const override { return false; }
    /** @brief whether the Charge Point will stop an ongoing transaction when it receives a non- Accepted authorization status in a
               StartTransaction.conf for this transaction */
    bool stopTransactionOnInvalidId() const override { return true; }
    /** @brief Clock-aligned periodic measurand(s) to be included in the TransactionData element of StopTransaction.req MeterValues.req
               PDU for every ClockAlignedDataInterval of the Transaction */
    std::string stopTxnAlignedData() const override { return ""; }
    /** @brief Maximum number of items in a StopTxnAlignedData Configuration Key. */
    unsigned int stopTxnAlignedDataMaxLength() const override { return 0; }
    /** @brief Sampled measurands to be included in the TransactionData element of StopTransaction.req PDU, every
               MeterValueSampleInterval seconds from the start of the charging session */
    std::string stopTxnSampledData() const override { return ""; }
    /** @brief Maximum number of items in a StopTxnSampledData Configuration Key.*/
    unsigned int stopTxnSampledDataMaxLength() const override { return 0; }
    /** @brief A list of supported Feature Profiles. Possible profile identifiers: Core, FirmwareManagement, LocalAuthListManagement,
               Reservation, SmartCharging and RemoteTrigger. */
    std::string supportedFeatureProfiles() const override { return ""; }
    /** @brief Maximum number of items in a SupportedFeatureProfiles Configuration Key. */
    unsigned int supportedFeatureProfilesMaxLength() const override { return 0; }
    /** @brief How often the Charge Point should try to submit a transaction-related message when the Central System fails to process it. */
    unsigned int transactionMessageAttempts() const override { return 0; }
    /** @brief How long the Charge Point should wait before resubmitting a transaction-related message that the Central System failed to
               process. */
    std::chrono::seconds transactionMessageRetryInterval() const override { return std::chrono::seconds(1); }
    /** @brief When set to true, the Charge Point SHALL unlock the cable on Charge Point side when the cable is unplugged at the EV */
    bool unlockConnectorOnEVSideDisconnect() const override { return true; }
    /** @brief Only relevant for websocket implementations. 0 disables client side websocket Ping/Pong. In this case there is either no
               ping/pong or the server initiates the ping and client responds with Pong. Positive values are interpreted as number of seconds
               between pings. Negative values are not allowed. ChangeConfiguration is expected to return a REJECTED result. */
    std::chrono::seconds webSocketPingInterval() const override { return std::chrono::seconds(1); }

    // Local Auth List Management Profile

    /** @brief whether the Local Authorization List is enabled */
    bool localAuthListEnabled() const override { return true; }
    /** @brief Maximum number of identifications that can be stored in the Local Authorization List */
    unsigned int localAuthListMaxLength() const override { return 0; }
    /** @brief Maximum number of identifications that can be send in a single SendLocalList.req */
    unsigned int sendLocalListMaxLength() const override { return 0; }

    // Reservation Profile

    /** @brief If this configuration key is present and set to true: Charge Point support reservations on connector 0. */
    bool reserveConnectorZeroSupported() const override { return true; }

    // Smart Charging Profile

    /** @brief Max StackLevel of a ChargingProfile. The number defined also indicates the max allowed number of installed charging
               schedules per Charging Profile Purposes. */
    unsigned int chargeProfileMaxStackLevel() const override { return 2u; }
    /** @brief A list of supported quantities for use in a ChargingSchedule. Allowed values: 'Current' and 'Power' */
    std::string chargingScheduleAllowedChargingRateUnit() const override { return ""; }
    /** @brief Maximum number of periods that may be defined per ChargingSchedule. */
    unsigned int chargingScheduleMaxPeriods() const override { return 0; }
    /** @brief If defined and true, this Charge Point support switching from 3 to 1 phase during a Transaction. */
    bool connectorSwitch3to1PhaseSupported() const override { return false; }
    /** @brief Maximum number of Charging profiles installed at a time */
    unsigned int maxChargingProfilesInstalled() const override { return 0; }

    //
    // Specific setters
    //

    /** @brief Interval of inactivity (no OCPP exchanges) with central system after which the Charge Point should send a Heartbeat.req PDU */
    void heartbeatInterval(std::chrono::seconds interval) override { (void)interval; }

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
    bool additionalRootCertificateCheck() const override { return true; }

    /** @brief The basic authentication password is used for HTTP Basic Authentication, minimal length: 16 bytes.
                It is strongly advised to be randomly generated binary to get maximal entropy. Hexadecimal represented (20 bytes maximum,
                represented as a string of up to 40 hexadecimal digits).
                This configuration key is write-only, so that it cannot be accidentally stored in plaintext by the Central System when it reads out
                all configuration keys.
                This configuration key is required unless only "security profile 3 - TLS with client side certificates" is implemented. */
    std::string authorizationKey() const override { return ""; }

    /** @brief This configuration key can be used to limit the size of the 'certificateChain' field from the CertificateSigned.req PDU. The value
               of this configuration key has a maximum limit of 10.000 characters. */
    unsigned int certificateSignedMaxChainSize() const override { return 0; }

    /** @brief Maximum number of Root/CA certificates that can be installed in the Charge Point. */
    unsigned int certificateStoreMaxLength() const override { return 0; }

    /** @brief This configuration key contains CPO name (or an organization trusted by the CPO) as used in the Charge Point Certificate. This
               is the CPO name that is to be used in a CSR send via: SignCertificate.req */
    std::string cpoName() const override { return ""; }

    /** @brief This configuration key is used to set the security profile used by the Charge Point.
               The value of this configuration key can only be increased to a higher level, not decreased to a lower level, if the Charge Point
               receives a lower value then currently configured,the Charge Point SHALL Rejected the ChangeConfiguration.req
               Before accepting the new value, the Charge Point SHALL check if all the prerequisites for the new Security Profile are met, if
               not, the Charge Point SHALL Rejected the ChangeConfiguration.req.
               After the security profile was successfully changed, the Charge Point disconnects from the Central System and SHALL
               reconnect using the new configured Security Profile.
               Default, when no security profile is yet configured: 0. */
    unsigned int securityProfile() const override { return 0; }

    /** @brief Comma separated list of supported file transfer protocols for upload AND download
               Allowed values : FTP, FTPS, HTTP, HTTPS, SFTP */
    std::string supportedFileTransferProtocols() const override { return ""; }
};

/** @brief Dummy implementation of charge point event handler */
class ChargePointEventsHandler : public IChargePointEventsHandler
{
  public:
    /** @brief Destructor */
    virtual ~ChargePointEventsHandler() { }

    /**
     * @brief Called when the first attempt to connect to the central system has failed
     * @param status Previous registration status (if Accepted, some offline operations are allowed)
     */
    void connectionFailed(ocpp::types::RegistrationStatus status) override { (void)status; }

    /**
     * @brief Called when the charge point connection status has changed
     * @param isConnected true if the charge point is connected to the central system, false otherwise
     */
    void connectionStateChanged(bool isConnected) override { (void)isConnected; }

    /**
     * @brief Called on boot notification response from the central system
     * @param status Registration status
     * @param datetime Date and time of the central system
     */
    void bootNotification(ocpp::types::RegistrationStatus status, const ocpp::types::DateTime& datetime) override
    {
        (void)status;
        (void)datetime;
    }

    /**
     * @brief Called when the date and time must be adjusted with the one of the central system
     */
    void datetimeReceived(const ocpp::types::DateTime& datetime) override { (void)datetime; }

    /**
     * @brief Called when a change availability request has been received from the central system
     * @param connector_id Id of the concerned connector (0 = whole charge point)
     * @param availability Requested availability
     * @return Status of the requested availability change (see AvailabilityStatus)
     */
    ocpp::types::AvailabilityStatus changeAvailabilityRequested(unsigned int                  connector_id,
                                                                ocpp::types::AvailabilityType availability) override
    {
        (void)connector_id;
        (void)availability;
        return AvailabilityStatus::Rejected;
    }

    /**
     * @brief Called to retrieve the meter value in Wh for a connector at the start or at the end of a transaction
     * @param connector_id Id of the concerned connector
     * @return Meter value in Wh for the connector
     */
    unsigned int getTxStartStopMeterValue(unsigned int connector_id) override
    {
        (void)connector_id;
        return 0;
    }

    /**
     * @brief Called when a reservation as started on a connector
     * @param connector_id Id of the concerned connector
     */
    void reservationStarted(unsigned int connector_id) override { (void)connector_id; }

    /**
     * @brief Called when a reservation as expired or has been canceled on a connector
     * @param connector_id Id of the concerned connector
     * @param canceled True is the reservation has been canceled, false if it has expired
     */
    void reservationEnded(unsigned int connector_id, bool canceled) override
    {
        (void)connector_id;
        (void)canceled;
    }

    /**
     * @brief Called when a data transfer request has been received
     * @param vendor_id Identifies the vendor specific implementation
     * @param message_id Identifies the message
     * @param request_data Data associated to the request
     * @param response_data Data associated with the response
     * @return Response status (see DataTransferStatus enum)
     */
    ocpp::types::DataTransferStatus dataTransferRequested(const std::string& vendor_id,
                                                          const std::string& message_id,
                                                          const std::string& request_data,
                                                          std::string&       response_data) override
    {
        (void)vendor_id;
        (void)message_id;
        (void)request_data;
        (void)response_data;
        return DataTransferStatus::Rejected;
    }

    /**
     * @brief Get a meter value associated to a connector
     * @param connector_id Id of the concerned connector (0 = whole charge point)
     * @param measurand Mesurand of the meter value to retrieve and its phase if specified
     * @param meter_value Meter value to fill (the context and measurand fields of SampleValues doesn't need to be filled)
     * @return true if the meter value can be retrived, false otherwise
     */
    bool getMeterValue(unsigned int                                                                        connector_id,
                       const std::pair<ocpp::types::Measurand, ocpp::types::Optional<ocpp::types::Phase>>& measurand,
                       ocpp::types::MeterValue&                                                            meter_value) override
    {
        (void)connector_id;
        (void)measurand;
        (void)meter_value;
        return false;
    }

    /**
     * @brief Called when a remote start transaction request has been received
     * @param connector_id Id of the concerned connector
     * @param id_tag Tag to use for the transaction
     * @return true if the request is accepted, false otherwise
     */
    bool remoteStartTransactionRequested(unsigned int connector_id, const std::string& id_tag) override
    {
        (void)connector_id;
        (void)id_tag;
        return false;
    }

    /**
     * @brief Called when a remote stop transaction request has been received
     * @param connector_id Id of the concerned connector
     * @return true if the request is accepted, false otherwise
     */
    bool remoteStopTransactionRequested(unsigned int connector_id) override
    {
        (void)connector_id;
        return false;
    }

    /**
     * @brief Called when a transaction which was started offline has been deauthorized
     *        when the connection with the Central System has been established again
     * @param connector_id Id of the concerned connector
     */
    void transactionDeAuthorized(unsigned int connector_id) override { (void)connector_id; }

    /**
     * @brief Called on a reset request from the Central System
     * @param reset_type Type of reset
     * @return true if the request is accepted, false otherwise
     */
    bool resetRequested(ocpp::types::ResetType reset_type) override
    {
        (void)reset_type;
        return false;
    }

    /**
     * @brief Called on an unlock connector request from the Central System
     * @param connector_id Id of the concerned connector
     * @return Unlock status (see UnlockStatus documentation)
     */
    ocpp::types::UnlockStatus unlockConnectorRequested(unsigned int connector_id) override
    {
        (void)connector_id;
        return UnlockStatus::NotSupported;
    }

    /**
     * @brief Called on a diagnostic request
     * @param start_time If set, contains the date and time of the oldest logging information to
     *                   include in the diagnostics
     * @param stop_time If set, contains the date and time of the latest logging information to
     *                  include in the diagnostics
     * @return Path to the generated diagnostic file to upload to the Central System, or empty
     *         string if no diagnostics are available
     */
    std::string getDiagnostics(const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                               const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) override
    {
        (void)start_time;
        (void)stop_time;
        return "";
    }

    /**
     * @brief Called on an update firmware request
     * @return Path where to download the firmware
     */
    std::string updateFirmwareRequested() override { return ""; }

    /**
     * @brief Called when a firmware is ready to be installed
     *        (The installation process can be done asynchronously after
     *        this function returns)
     * @param firmware_file Path to the firmware file to install
     */
    void installFirmware(const std::string& firmware_file) override { (void)firmware_file; }

    /**
     * @brief Called to upload a file to the a given URL
     * @param file Path to the file to upload
     * @param url URL where to upload the file
     * @return true if the file has been uploaded, false otherwise
     */
    bool uploadFile(const std::string& file, const std::string& url) override
    {
        (void)file;
        (void)url;
        return false;
    }

    /**
     * @brief Called to download a file from the a given URL
     * @param url URL from where to download the file
     * @param file Path where to save the file to download
     * @return true if the file has been downloaded, false otherwise
     */
    bool downloadFile(const std::string& url, const std::string& file) override
    {
        (void)url;
        (void)file;
        return false;
    }

    // Security extensions

    /**
     * @brief Called when a CA certificate has been received and must be installed
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @param type Type of CA certificate
     * @param certificate CA certificate to install
     * @return Installation status (see CertificateStatusEnumType enum)
     */
    ocpp::types::CertificateStatusEnumType caCertificateReceived(ocpp::types::CertificateUseEnumType type,
                                                                 const ocpp::x509::Certificate&      certificate) override
    {
        (void)type;
        (void)certificate;
        return CertificateStatusEnumType::Rejected;
    }

    /**
     * @brief Called when a charge point certificate has been received and must be installed
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @param certificate Charge point certificate to install
     * @return true is the certificate has been installed, false otherwise
     */
    bool chargePointCertificateReceived(const ocpp::x509::Certificate& certificate) override
    {
        (void)certificate;
        return false;
    }

    /**
     * @brief Called when the Central System request to delete an installed CA certificate
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @param hash_algorithm Hash algorithm used for the following parameters
     * @param issuer_name_hash Hash of the certificate's issuer's name
     * @param issuer_key_hash Hash of the certificate's public key
     * @param serial_number Serial number of the certificate
     * @return Deletion status (see DeleteCertificateStatusEnumType enum)
     */
    ocpp::types::DeleteCertificateStatusEnumType deleteCertificate(ocpp::types::HashAlgorithmEnumType hash_algorithm,
                                                                   const std::string&                 issuer_name_hash,
                                                                   const std::string&                 issuer_key_hash,
                                                                   const std::string&                 serial_number) override
    {
        (void)hash_algorithm;
        (void)issuer_name_hash;
        (void)issuer_key_hash;
        (void)serial_number;
        return DeleteCertificateStatusEnumType::Failed;
    }

    /**
     * @brief Called to generate a CSR in PEM format which will be used by the Central System
     *        to generate and sign a certificate for the Charge Point
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @param csr String to store the generated CSR in PEM format
     */
    void generateCsr(std::string& csr) override { (void)csr; }

    /**
     * @brief Called to get the list of installed CA certificates
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @param type Type of CA certificate
     * @param certificates Installed certificates
     */
    void getInstalledCertificates(ocpp::types::CertificateUseEnumType type, std::vector<ocpp::x509::Certificate>& certificates) override
    {
        (void)type;
        (void)certificates;
    }

    /**
     * @brief Called on a log request
     * @param type Type of log to upload
     * @param start_time If set, contains the date and time of the oldest logging information to
     *                   include in the log file
     * @param stop_time If set, contains the date and time of the latest logging information to
     *                  include in the log file
     * @return Path to the generated log file to upload to the Central System, or empty
     *         string if no log are available
     */
    std::string getLog(ocpp::types::LogEnumType                            type,
                       const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                       const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) override
    {
        (void)type;
        (void)start_time;
        (void)stop_time;
        return "";
    }

    /**
     * @brief Called to check if at least 1 Central System root certificate has been installed
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @return true if at least 1 certificate has been installed, false otherwise
     */
    bool hasCentralSystemCaCertificateInstalled() override { return false; }

    /**
     * @brief Called to check if at least 1 Charge Point certificate has been installed
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @return true if at least 1 certificate has been installed, false otherwise
     */
    bool hasChargePointCertificateInstalled() override { return false; }

    /**
     * @brief Called to check the firmware signing certificate against installed Manufacturer CA certificates
     *        (Not used if InternalCertificateManagementEnabled = true)
     * @param signing_certificate Certificate to check
     * @return Check status (see UpdateFirmwareStatusEnumType enum)
     */
    ocpp::types::UpdateFirmwareStatusEnumType checkFirmwareSigningCertificate(const ocpp::x509::Certificate& signing_certificate) override
    {
        (void)signing_certificate;
        return UpdateFirmwareStatusEnumType::Rejected;
    }
};

/** @brief Entry point */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Instanciate a central system object
    CentralSystemConfig             cs_config;
    CentralSystemEventsHandler      cs_event_handler;
    std::unique_ptr<ICentralSystem> central_system = ICentralSystem::create(cs_config, cs_event_handler);
    central_system->start();

    // Instanciate a charge point object
    ChargePointConfig             cp_config;
    OcppConfig                    ocpp_config;
    ChargePointEventsHandler      cp_event_handler;
    std::unique_ptr<IChargePoint> charge_point = IChargePoint::create(cp_config, ocpp_config, cp_event_handler);
    charge_point->start();

    return 0;
}
