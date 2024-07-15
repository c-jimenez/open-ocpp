# Open OCPP 1.6

## Table of contents

- [Open OCPP 1.6](#open-ocpp-1.6)
  - [Table of contents](#table-of-contents)
  - [Features](#features)
    - [Key features](#key-features)
    - [Supported OCPP feature profiles](#supported-ocpp-feature-profiles)
    - [Supported OCPP configuration keys](#supported-ocpp-configuration-keys)
    - [OCPP security extensions](#ocpp-security-extensions)
      - [Security profiles](#security-profiles)
      - [Security events](#security-events)
      - [Extended trigger messages](#extended-trigger-messages)
      - [Certificate management](#certificate-management)
      - [Signed firmware update](#signed-firmware-update)
    - [OCPP ISO15118 PnC extensions](#ocpp-iso15118-pnc-extensions)
      - [Charge Point role](#charge-point-role)
      - [Central System role](#central-system-role)
    - [Internal configuration keys](#internal-configuration-keys)
      - [Common keys](#common-keys)
      - [Charge Point keys](#charge-point-keys)
      - [Central System keys](#central-system-keys)
      - [Local Controller keys](#local-controller-keys)
  - [Quick start](#quick-start)
    - [Charge Point role](#charge-point-role-1)
      - [Configuration interface](#configuration-interface)
      - [Event handler interface](#event-handler-interface)
      - [Charge Point object](#charge-point-object)
    - [Central System role](#central-system-role-1)
      - [Configuration interface](#configuration-interface-1)
      - [Event handler interfaces](#event-handler-interfaces)
      - [Central System object](#central-system-object)

## Features

### Key features
**Open OCPP** is composed of the 4 layers defined by the protocol :

* Websockets (Client or Server)
* OCPP-J RPC
* JSON messages serialization/deserialization
* OCPP role (Charge Point, Central System or Local Controller)

As of this version :

* All the messages defined in the OCPP 1.6 edition 2 protocol have been implemented
* All the configuration keys defined in the OCPP 1.6 edition 2 protocol have been implemented for the Charge Point role
* All the messages defined in the OCPP 1.6 security whitepaper edition 2 have been implemented
* All the messages defined in the Using ISO 15118 Plug & Charge with OCPP 1.6 Application Note v1.0 have been implemented

The user application will have to implement some callbacks to provide the data needed by **Open OCPP** or to handle OCPP events (boot notification, remote start/stop notifications, meter values...).

The persistent data handled by **Open OCPP** is stored into a single file which is an [SQLite](https://www.sqlite.org/) database. It contains :

* For Charge Point role :

  + Internal configuration
  + Persistent data : Central System's registration status, connector state, OCPP transaction related messages when offline, StopTx meter values
  + Badge cache and local list
  + Smart charging profile
  + Logs
  * X.509 Certificates

* For Central System or Local Controller role :

  + Internal configuration
  + Logs

The standard OCPP configuration persistency has to be handled by the user application.

### Supported OCPP feature profiles

|          Profile           | Description                                                                                                                                                    | Restrictions                                                                                                                                    |
| :------------------------: | :------------------------------------------------------------------------------------------------------------------------------------------------------------- | :---------------------------------------------------------------------------------------------------------------------------------------------- |
|            Core            | Basic Charge Point functionality comparable with OCPP 1.5 [OCPP1.5] without support for firmware updates, local authorization list management and reservations | OCPP confguration persistency has to be handled by the user application                                                                         |
|    Firmware Management     | Support for firmware update management and diagnostic log file download                                                                                        | Actual file download/upload as well as firmware installation must be handled by the user application in the callbacks provided by **Open OCPP** |
| Local Auth List Management | Features to manage the local authorization list in Charge Points                                                                                               | None                                                                                                                                            |
|        Reservation         | Support for reservation of a Charge Point.                                                                                                                     | None                                                                                                                                            |
|       Smart Charging       | Support for basic Smart Charging, for instance using control pilot                                                                                             | GetCompositeSchedule is not supported for connector 0 in Charge Point role                                                                      |
|       Remote Trigger       | Support for remote triggering of Charge Point initiated messages                                                                                               | None                                                                                                                                            |

### Supported OCPP configuration keys

The OCPP configuration keys support applies to Charge Point role only.

In the "Owner" column, "S" means that the configuration key behavior is handled by the stack, "U" means that it must handled by the user application.

|                   Key                   | Owner | Restrictions                                                                                                                                                                                                                           |
| :-------------------------------------: | :---: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|       AllowOfflineTxForUnknownId        |   S   | None                                                                                                                                                                                                                                   |
|        AuthorizationCacheEnabled        |   S   | None                                                                                                                                                                                                                                   |
|        AuthorizeRemoteTxRequests        |   U   | None                                                                                                                                                                                                                                   |
|               BlinkRepeat               |   U   | None                                                                                                                                                                                                                                   |
|        ClockAlignedDataInterval         |   S   | None                                                                                                                                                                                                                                   |
|            ConnectionTimeOut            |   U   | None                                                                                                                                                                                                                                   |
|         ConnectorPhaseRotation          |   U   | None                                                                                                                                                                                                                                   |
|     ConnectorPhaseRotationMaxLength     |   U   | None                                                                                                                                                                                                                                   |
|         GetConfigurationMaxKeys         |   S   | Must be set to the sum of OCPP configuration keys count (49) + user application configuration keys count to allow to export all the configuration in 1 message                                                                         |
|            HeartbeatInterval            |   S   | Heartbeat are only sent if no messages have been exchanged since HeartbeatInterval seconds                                                                                                                                             |
|             LightIntensity              |   U   | None                                                                                                                                                                                                                                   |
|          LocalAuthorizeOffline          |   S   | None                                                                                                                                                                                                                                   |
|            LocalPreAuthorize            |   S   | None                                                                                                                                                                                                                                   |
|          MaxEnergyOnInvalidId           |   U   | None                                                                                                                                                                                                                                   |
|         MeterValuesAlignedData          |  S+U  | User application must validate the requested meter value list according to its metering capacities when modified by the Central System                                                                                                 |
|     MeterValuesAlignedDataMaxLength     |   S   | None                                                                                                                                                                                                                                   |
|         MeterValuesSampledData          |  S+U  | User application must validate the requested meter value list according to its metering capacities when modified by the Central System                                                                                                 |
|     MeterValuesSampledDataMaxLength     |   S   | None                                                                                                                                                                                                                                   |
|        MeterValueSampleInterval         |   S   | None                                                                                                                                                                                                                                   |
|          MinimumStatusDuration          |   S   | None                                                                                                                                                                                                                                   |
|           NumberOfConnectors            |   S   | None                                                                                                                                                                                                                                   |
|              ResetRetries               |   U   | None                                                                                                                                                                                                                                   |
|    StopTransactionOnEVSideDisconnect    |   U   | None                                                                                                                                                                                                                                   |
|       StopTransactionOnInvalidId        |   U   | None                                                                                                                                                                                                                                   |
|           StopTxnAlignedData            |  S+U  | User application must validate the requested meter value list according to its metering capacities when modified by the Central System                                                                                                 |
|       StopTxnAlignedDataMaxLength       |   S   | None                                                                                                                                                                                                                                   |
|           StopTxnSampledData            |  S+U  | User application must validate the requested meter value list according to its metering capacities when modified by the Central System                                                                                                 |
|       StopTxnSampledDataMaxLength       |   S   | None                                                                                                                                                                                                                                   |
|        SupportedFeatureProfiles         |   S   | None                                                                                                                                                                                                                                   |
|    SupportedFeatureProfilesMaxLength    |   S   | None                                                                                                                                                                                                                                   |
|       TransactionMessageAttempts        |   S   | None                                                                                                                                                                                                                                   |
|     TransactionMessageRetryInterval     |   S   | None                                                                                                                                                                                                                                   |
|    UnlockConnectorOnEVSideDisconnect    |   U   | None                                                                                                                                                                                                                                   |
|          WebSocketPingInterval          |   S   | Reboot required                                                                                                                                                                                                                        |
|          LocalAuthListEnabled           |   S   | None                                                                                                                                                                                                                                   |
|         LocalAuthListMaxLength          |   S   | None                                                                                                                                                                                                                                   |
|         SendLocalListMaxLength          |   S   | None                                                                                                                                                                                                                                   |
|      ReserveConnectorZeroSupported      |   S   | None                                                                                                                                                                                                                                   |
|       ChargeProfileMaxStackLevel        |   S   | None                                                                                                                                                                                                                                   |
| ChargingScheduleAllowedChargingRateUnit |   S   | None                                                                                                                                                                                                                                   |
|       ChargingScheduleMaxPeriods        |   S   | None                                                                                                                                                                                                                                   |
|    ConnectorSwitch3to1PhaseSupported    |   S   | None                                                                                                                                                                                                                                   |
|      MaxChargingProfilesInstalled       |   S   | None                                                                                                                                                                                                                                   |
|     AdditionalRootCertificateCheck      |  U/S  | If internal certificate management is enabled, the stack handle this parameter (implemented behavior for now is the always the one corresponding to AdditionalRootCertificateCheck = False), otherwise it must be the user application |
|            AuthorizationKey             |   S   | None                                                                                                                                                                                                                                   |
|      CertificateSignedMaxChainSize      |   S   | None                                                                                                                                                                                                                                   |
|        CertificateStoreMaxLength        |  U/S  | If internal certificate management is enabled, the stack handle this parameter, otherwise it must be the user application                                                                                                              |
|                 CpoName                 |   S   | None                                                                                                                                                                                                                                   |
|             SecurityProfile             |   S   | None                                                                                                                                                                                                                                   |
|     SupportedFileTransferProtocols      |   U   | None                                                                                                                                                                                                                                   |
|    CentralContractValidationAllowed     |   S   | None                                                                                                                                                                                                                                   |
|         CertSigningWaitMinimum          |   S   | None                                                                                                                                                                                                                                   |
|         CertSigningRepeatTimes          |   S   | None                                                                                                                                                                                                                                   |
|        ContractValidationOffline        |  U/S  | The stack will notify the user application for contract validation depending on the value of this parameter                                                                                                                            |
|           ISO15118PnCEnabled            |   S   | None                                                                                                                                                                                                                                   |

### OCPP security extensions

#### Security profiles

**Open OCPP** support the following Security Profiles for both Charge Point and Central System roles :

* 0 : No security profile
* 1 : Unsecured Transport with HTTP Basic Authentication
* 2 : TLS with HTTP Basic Authentication
* 3 : TLS with Client Side Certificates

In Charge Point role, the stack will automatically disconnect and then reconnect using the new parameters to the Central System after one of the following parameters has been modified :
* **AuthorizationKey**
* **Security Profile**

**Restriction** : The automatic fallback to old connection parameters if the connection fails after switching to a new security is not implemented yet.

#### Security events

**Open OCPP** support the whole use cases of security events and logging.

In Charge Point role, it can optionnaly handle the storage of the security event log and the generation of the security log export when the Central System asks it. To enable/disable this feature, you have to modify the **SecurityLogMaxEntriesCount** charge point configuration key :

* 0 = **Open OCPP** will not store security event and the security log must be generated by the user application
* \>0 = **Open OCPP** will store at max **SecurityLogMaxEntriesCount** (circular log) and will automatically generate the security log as a CSV file

In Charge Point role, the user application can generate custom security events and defines its criticity so that they are forwarded to the Central System.

In Charge Point role, the notification of security events can be enabled or disabled with the **SecurityEventNotificationEnabled** configuration key. This can be usefull to disable them when the Central System does not implement the security extensions.

#### Extended trigger messages

**Open OCPP** support this feature for both Charge Point and Central System roles.

#### Certificate management

**Open OCPP** support this feature for both Charge Point and Central System roles.

The behavior of this feature is controlled by the **InternalCertificateManagementEnabled** configuration key.

If **InternalCertificateManagementEnabled** is set to **false**, the actual storage of the certificates and their keys must be done by the user application. **Open OCPP** provides callbacks and helper classes to ease certificate manipulation and installation. The user application also has to configure the path to the installed certificates for the establishment of the secure connections using the following configuration keys :

* TlsServerCertificateCa
* TlsClientCertificate
* TlsClientCertificatePrivateKey
* TlsClientCertificatePrivateKeyPassphrase

If **InternalCertificateManagementEnabled** is set to **true**, the storage of certificates and their keys is fully handled by **Open OCPP**. The user application just has to provide a passphrase using the **TlsClientCertificatePrivateKeyPassphrase** configuration key to securily encrypt the certicates' private keys using AES-256-CBC algorithm. **Open OCPP** will automatically use the installed corresponding certificates depending on the configured Security Profile and the certificates validity dates.

**Restriction** : The automatic fallback to old certificate if the connection fails after installing new certificate is not implemented yet.

#### Signed firmware update

**Open OCPP** support this feature for both Charge Point and Central System roles.

**Open OCPP** provides helper classes based on OpenSSL to ease private keys, certificate and certificate requests usage : generation, signature, verification. They can be used in the user application callbacks. These helpers can be found in the ocpp::tools::x509 namespace and are widely used in the **Open OCPP** source code and examples.

### OCPP ISO15118 PnC extensions

**Open OCPP** fully supports the whole messaging, data types and configuration keys set associated to the ISO15118 PnC extensions.

#### Charge Point role

In Charge Point role these extensions consists mainly on forwarding messages from the ISO15118-2 stack layer to the Central System by using dedicated DataTransfer messages.

**Open OCPP** implements the forwarding and provides callback and retries capabilities for certificates messages.

Allthough **Open OCPP** is able to manage a certificate store, the Charge Point certificate used for ISO15118 communication won't be stored in it even if the **InternalCertificateManagementEnabled** configuration key is set to **true**. This is will allow for the ISO15118-2 stack to access this certificate and use it to secure its communications with the vehicule.

#### Central System role

In Central System role these extensions consists mainly in certificate management by forwarding request either to OCSP server or Mobility Operators.

**Open OCPP** provides callbacks where the forwarding to the necessary servers must be implemented.

### Internal configuration keys

The behavior and the configuration of the **Open OCPP** stack can be modified through configuration keys. Some are specific to an OCPP role and some are common.

#### Common keys

|        Key         |  Type  | Description                                                         |
| :----------------: | :----: | :------------------------------------------------------------------ |
|    DatabasePath    | string | Path to the database to store persistent data                       |
|  JsonSchemasPath   | string | Path to the JSON schemas to validate the messages                   |
| CallRequestTimeout |  uint  | Call request timeout in milliseconds                                |
|  Tlsv12CipherList  | string | List of authorized ciphers for TLSv1.2 connections (OpenSSL format) |
|  Tlsv13CipherList  | string | List of authorized ciphers for TLSv1.3 connections (OpenSSL format) |
| LogMaxEntriesCount |  uint  | Maximum number of entries in the log (0 = no logs in database)      |

#### Charge Point keys

|                       Key                       |  Type  | Description                                                                                                                                                                                           |
| :---------------------------------------------: | :----: | :---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|                  ConnexionUrl                   | string | URL of the Central System                                                                                                                                                                             |
|              ChargePointIdentifier              | string | OCPP Charge Point identifier. Will be concatanated with the **ConnexionUrl** key                                                                                                                      |
|                ConnectionTimeout                |  uint  | Connection timeout in milliseconds                                                                                                                                                                    |
|                  RetryInterval                  |  uint  | Retry interval when connection has failed in milliseconds                                                                                                                                             |
|              ChargeBoxSerialNumber              | string | Deprecated. Charge Box serial number for BootNotification message                                                                                                                                     |
|                ChargePointModel                 | string | Charge Point model for BootNotification message                                                                                                                                                       |
|             ChargePointSerialNumber             | string | Charge Point serial number for BootNotification message                                                                                                                                               |
|                ChargePointVendor                | string | Charge Point vendor for BootNotification message                                                                                                                                                      |
|                 FirmwareVersion                 | string | Charge Point firmware version for BootNotification message                                                                                                                                            |
|                      Iccid                      | string | ICCID of the moden's SIM card for BootNotification message                                                                                                                                            |
|                      Imsi                       | string | IMSI of the moden's SIM card for BootNotification message                                                                                                                                             |
|                MeterSerialNumber                | string | Main electrical meter serial number for BootNotification message                                                                                                                                      |
|                    MeterType                    | string | Main electrical meter type for BootNotification message                                                                                                                                               |
|                OperatingVoltage                 | float  | Nominal operating voltage (needed for Watt to Amp conversions in smart charging profiles)                                                                                                             |
|           AuthentCacheMaxEntriesCount           |  uint  | Maximum number of entries in the authentication cache                                                                                                                                                 |
|             TlsServerCertificateCa              | string | Path to Certification Authority signing chain to validate the Central System certificate                                                                                                              |
|              TlsClientCertificate               | string | Path to Charge Point certificate                                                                                                                                                                      |
|         TlsClientCertificatePrivateKey          | string | Path to Charge Point's certificate's private key                                                                                                                                                      |
|    TlsClientCertificatePrivateKeyPassphrase     | string | Charge Point certificate's private key passphrase                                                                                                                                                     |
|         TlsAllowSelfSignedCertificates          |  bool  | Allow TLS connections using self-signed certificates (Warning : enabling this feature is not recommended in production)                                                                               |
|           TlsAllowExpiredCertificates           |  bool  | Allow TLS connections using expired certificates (Warning : enabling this feature is not recommended in production)                                                                                   |
|         TlsAcceptNonTrustedCertificates         |  bool  | Accept non trusted certificates for TLS connections (Warning : enabling this feature is not recommended in production)                                                                                |
|             TlsSkipServerNameCheck              |  bool  | Skip server name check in certificates for TLS connections (Warning : enabling this feature is not recommended in production)                                                                         |
|      InternalCertificateManagementEnabled       |  bool  | If true, certificates are stored inside **Open OCPP** databasen otherwise user application has to handle them                                                                                         |
|        SecurityEventNotificationEnabled         |  bool  | Enable security event notification                                                                                                                                                                    |
|           SecurityLogMaxEntriesCount            |  uint  | Maximum number of entries in the security log (0 = no security logs in database)                                                                                                                      |
|        ClientCertificateRequestHashType         | string | Hash type for certificate request generation : sha256, sha384 or sha512                                                                                                                               |
|         ClientCertificateRequestKeyType         | string | Key type for certificate request generation : ec or rsa                                                                                                                                               |
|      ClientCertificateRequestRsaKeyLength       |  uint  | Length in bits of the key for certificate request generation if rsa has been selected for key type : minimum 2048                                                                                     |
|         ClientCertificateRequestEcCurve         | string | Name of the elliptic curve for certificate request generation if ec has been selected for key type : prime256v1, secp256k1, secp384r1, secp521r1, brainpoolP256t1, brainpoolP384t1 or brainpoolP512t1 |
|     ClientCertificateRequestSubjectCountry      | string | Country for the subject field of certificate request generation (can be left empty)                                                                                                                   |
|      ClientCertificateRequestSubjectState       | string | State for the subject field of certificate request generation (can be left empty)                                                                                                                     |
|     ClientCertificateRequestSubjectLocation     | string | Location for the subject field of certificate request generation (can be left empty)                                                                                                                  |
| ClientCertificateRequestSubjectOrganizationUnit | string | Organization unit for the subject field of certificate request generation (can be left empty)                                                                                                         |
|      ClientCertificateRequestSubjectEmail       | string | Email for the subject field of certificate request generation (can be left empty)                                                                                                                     |

#### Central System keys

|                   Key                    |  Type  | Description                                                                                                  |
| :--------------------------------------: | :----: | :----------------------------------------------------------------------------------------------------------- |
|                ListenUrl                 | string | URL to listen to incomming  websocket connections                                                            |
|          WebSocketPingInterval           |  uint  | Websocket PING interval in seconds                                                                           |
|      BootNotificationRetryInterval       |  uint  | Boot notification retry interval in second (sent in BootNotificationConf when status is Pending or Rejected) |
|            HeartbeatInterval             |  uint  | Heartbeat interval in seconds (sent in BootNotificationConf when status is Accepted)                         |
|             HttpBasicAuthent             |  bool  | If set to true, the Charge Points must autenticate themselves using HTTP Basic Authentication method         |
|               TlsEcdhCurve               | string | ECDH curve to use for TLS connections with EC keys                                                           |
|           TlsServerCertificate           | string | Path to the Central System's certificate                                                                     |
|      TlsServerCertificatePrivateKey      | string | Path to the Central System's certificate's private key                                                       |
| TlsServerCertificatePrivateKeyPassphrase | string | Central System's certificate's private key passphrase                                                        |
|          TlsServerCertificateCa          | string | Path to the Certification Authority signing chain for the Central System's certificate                       |
|       TlsClientCertificateAuthent        |  bool  | If set to true, the Charge Points must authenticate themselves using an X.509 certificate                    |

#### Local Controller keys

|                   Key                    |  Type  | Description                                                                                                                               |
| :--------------------------------------: | :----: | :---------------------------------------------------------------------------------------------------------------------------------------- |
|                ListenUrl                 | string | URL to listen to incomming  websocket connections                                                                                         |
|          WebSocketPingInterval           |  uint  | Websocket PING interval in seconds                                                                                                        |
|             HttpBasicAuthent             |  bool  | If set to true, the Charge Points must autenticate themselves using HTTP Basic Authentication method                                      |
|               TlsEcdhCurve               | string | ECDH curve to use for TLS connections with EC keys                                                                                        |
|           TlsServerCertificate           | string | Path to the Central System's certificate                                                                                                  |
|      TlsServerCertificatePrivateKey      | string | Path to the Central System's certificate's private key                                                                                    |
| TlsServerCertificatePrivateKeyPassphrase | string | Central System's certificate's private key passphrase                                                                                     |
|          TlsServerCertificateCa          | string | Path to the Certification Authority signing chain for the Central System's certificate                                                    |
|       TlsClientCertificateAuthent        |  bool  | If set to true, the Charge Points must authenticate themselves using an X.509 certificate                                                 |
|    DisconnectFromCpWhenCsDisconnected    |  bool  | If set to true, the Charge Point is automatically disconnected when the connection to the Central System cannot be established or is lost |

## Quick start

The best way to start is to take a look at the [examples](./examples/README.md) and more specifically at the [quick start Charge Point example](./examples/ocpp16/quick_start_chargepoint/README.md), the [quick start Central System example](./examples/ocpp16/quick_start_centralsystem/README.md) and the [quick start Local Controller example](./examples/ocpp16/quick_start_localcontroller/README.md).

### Charge Point role

The implementation of a program using **Open OCPP** in Charge Point role is done in 3 steps :
* Implementation of the configuration interfaces [IOcppConfig](./src/ocpp16/config/IOcppConfig.h) and [IChargePointConfig](./src/ocpp16/chargepoint/interface/IChargePointConfig.h)
* Implementation of the event handler interface [IChargePointEventsHandler](./src/ocpp16/chargepoint/interface/IChargePointEventsHandler.h)
* Instanciation and use of the Charge Point object [IChargePoint](./src/ocpp16/chargepoint/interface/IChargePoint.h)

#### Configuration interface

The configuration interface allow **Open OCPP** to access to the values of the standard OCPP configuration keys and to the user application specific configuration keys.

The persistency of OCPP configuration key is not handled by **Open OCPP** for 2 main reasons :

* The user application will surely already have a configuration management component
* The user application may have to access the OCPP configuration keys or may want to export its own keys already managed by its configuration component

The configuration interface is split in 2 parts :

* IOcppConfig : interface to access OCPP configuration keys and user application specific configuration keys
* IChargePointConfig : interface to access Charge Point configuration (database paths, url, ...)

In the examples, the interfaces have been implemented to retrieve the values from a file stored into an INI format. This is a simple implementaton which is good to show how to implement these interfaces but which is not the most optimized one since every access to a configuration value implies a conversion from a string. Try to have a better implementation to boost the performances of the software ;)

#### Event handler interface

Most of the OCPP behavior is handled by **Open OCPP** but to complete the implementation of the OCPP standard, some information may be needed by **Open OCPP** (Meter values, change availability permissions...) or some operations may be done by the user application (File upload/download, firmware install...).

The event handler interface defines all these interaction as well as some usefull notifications to the user application : connection status, registration status, reservation status...

Most of the notifications/operations can be left empty if the corresponding OCPP feature is not used by the user application. See inside the examples how to implement unused functionalities.

Please keep in mind that all the calls to the event handler interface are made from different threads managed by **Open OCPP** depending the kind of notification/operation and that the treatment of theses calls must not be blocking (except for file upload/download) since it will have an impact to the global scheduling of **Open OCPP**.

#### Charge Point object

This is the easiest part :)

The Charge Point object is instanciated through a factory interface :

```
/**
 * @brief Instanciate a charge point
 * @param stack_config Stack configuration
 * @param ocpp_config Standard OCPP configuration
 * @param event_handler Stack event handler
 */
static std::unique_ptr<IChargePoint> create(const ocpp::config::IChargePointConfig& stack_config,
                                            ocpp::config::IOcppConfig&              ocpp_config,
                                            IChargePointEventsHandler&              events_handler);
```

The 3 parameters are the instances of the interfaces you have implemented in the previous steps.

Before starting the Charge Point object and thus the OCPP stack with the ```start()``` method, you can clear existing persistent data using the following methods:

* ```resetConnectorData()``` : clear all connector related data
* ```resetData()``` : clear all the persistent data

Once the Charge Point object has been started, **Open OCPP** will continuously try to connect/keep alive the communication with the Central System until a call to the ```stop()``` method which will disconnect and release the connection.

Connectivity status, registration status and Central System initiated operations will be notified through the event handler interface.

OCPP Charge Point operations are triggered by the Charge Point object interface.

Extract of a quick start main() :

```
int main()
{
    // Configuration
    ChargePointDemoConfig config("config.ini");

    // Event handler
    DefaultChargePointEventsHandler event_handler(config);

    // Instanciate charge point
    std::unique_ptr<IChargePoint> charge_point = IChargePoint::create(config.stackConfig(), config.ocppConfig(), event_handler);
    charge_point->start();

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        // Wait to be accepted by Central System
        while (charge_point->getRegistrationStatus() != RegistrationStatus::Accepted)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100u));
        }

        // Ask for authorization on a tag
        std::string               parent_id;
        std::string id_tag = "AABBCCDDEEFF";
        AuthorizationStatus status = charge_point->authorize(connector_id, id_tag, parent_id);
        if (status == AuthorizationStatus::Accepted)
        {
            std::cout << "Id tag authorized, parent id = " << parent_id << std::endl;

            // Preparing state
            charge_point->statusNotification(connector_id, ChargePointStatus::Preparing);
            std::this_thread::sleep_for(std::chrono::seconds(1u));

            // Try to start charging session
            status = charge_point->startTransaction(connector_id, id_tag);
            if (status == AuthorizationStatus::Accepted)
            {
                std::cout << "Transaction authorized, start charging" << std::endl;

                // Charging state
                charge_point->statusNotification(connector_id, ChargePointStatus::Charging);
                std::this_thread::sleep_for(std::chrono::seconds(30u));

                // End transaction
                charge_point->stopTransaction(connector_id, id_tag, Reason::Local);

                // Finishing state
                charge_point->statusNotification(connector_id, ChargePointStatus::Finishing);
                std::this_thread::sleep_for(std::chrono::seconds(1u));
            }
            else
            {
                std::cout << "Transaction not authorized by Central System : " << AuthorizationStatusHelper.toString(status)
                            << std::endl;
            }

            // Available state
            charge_point->statusNotification(connector_id, ChargePointStatus::Available);
        }
        else
        {
            std::cout << "Id tag not authorized by Central System : " << AuthorizationStatusHelper.toString(status) << std::endl;
        }

        // Wait before next charging session
        std::this_thread::sleep_for(std::chrono::seconds(10u));
    }

    return 0;
}
```

### Central System role

The implementation of a program using **Open OCPP** in Central System role is done in 3 steps :
* Implementation of the configuration interface [ICentralSystemConfig](./src/ocpp16/centralsystem/interface/ICentralSystemConfig.h)
* Implementation of the event handler interfaces [ICentralSystemEventsHandler](./src/ocpp16/centralsystem/interface/ICentralSystemEventsHandler.h) and [IChargePointRequestHandler](./src/ocpp16/centralsystem/interface/IChargePointRequestHandler.h)
* Instanciation and use of the Central System object [ICentralSystem](./src/ocpp16/centralsystem/interface/ICentralSystem.h)

#### Configuration interface

The configuration interface allow **Open OCPP** to access to its configuration values.
The persistency of the configuration is not handled by **Open OCPP** for 2 main reasons :

* The user application will surely already have a configuration management component
* The user application may have to access the **Open OCPP** configuration

In the examples, the interface has been implemented to retrieve the values from a file stored into an INI format. This is a simple implementaton which is good to show how to implement this interface but which is not the most optimized one since every access to a configuration value implies a conversion from a string. Try to have a better implementation to boost the performances of the software ;)

#### Event handler interfaces

In Central System role, the OCPP behavior must be implemented by the user application. **Open OCPP** handles all the other layers of the stack (websocket, RPC, JSON serialization/deserialization).

The **Open OCPP** stack will interact with the user application through 2 interfaces :

* [ICentralSystemEventsHandler](./src/ocpp16/centralsystem/interface/ICentralSystemEventsHandler.h) : used for all connection related events (credentials check, connection notification...)
* [IChargePointRequestHandler](./src/ocpp16/centralsystem/interface/IChargePointRequestHandler.h) : used to handle incoming requests from a Charge Point (boot notification, status notification, start transaction...)

The **ICentralSystemEventsHandler** must be instanciated only once for a Central System implementation.

The **IChargePointRequestHandler** must be instanciated and registered to each connected Charge Point in the **ICentralSystemEventsHandler::chargePointConnected** method implementation.

Example of Charge Point connection handling :

```
/** @copydoc bool ICentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ICentralSystem::IChargePoint>) */
void MyCentralSystemEventsHandler::chargePointConnected(std::shared_ptr<ocpp::centralsystem::ICentralSystem::IChargePoint> chargepoint)
{
    cout << "Charge point [" << chargepoint->identifier() << "] connected" << endl;
    auto iter_chargepoint = m_chargepoints.find(chargepoint->identifier());
    if (iter_chargepoint == m_chargepoints.end())
    {
        MyChargePointRequestHandler* my_handler = new MyChargePointRequestHandler(*this, chargepoint);
        chargepoint->registerHandler(*my_handler);
        m_chargepoints[chargepoint->identifier()] =
            std::shared_ptr<ChargePointRequestHandler>(my_handler);
    }
    else
    {
        cout << "Charge point [" << chargepoint->identifier() << "] already connected" << endl;
        chargepoint.reset();
    }
}
```

Please keep in mind that all the calls to the event handler interface are made from different threads managed by **Open OCPP** depending the kind of notification/operation and that the treatment of theses calls must not be blocking since it will have an impact to the global scheduling of **Open OCPP**.

#### Central System object

This is the easiest part :)

The Central System object is instanciated through a factory interface :

```
/**
 * @brief Instanciate a central system
 * @param stack_config Stack configuration
 * @param event_handler Stack event handler
*/
static std::unique_ptr<ICentralSystem> create(const ocpp::config::ICentralSystemConfig& stack_config,
                                              ICentralSystemEventsHandler&              events_handler);
```

The 2 parameters are the instances of the interfaces you have implemented in the previous steps.

Before starting the Central System object and thus the OCPP stack with the ```start()``` method, you can clear existing persistent data using the following method:

```resetData()``` : clear all the persistent data

Once the Central System object has been started, **Open OCPP** will continuously listen to incoming connections from the Charge Points until a call to the ```stop()``` method which will disconnect all the Charge Points and release the connection.

Connectivity status and Charge Point initiated operations will be notified through the event handler interfaces.

OCPP Central System operations are triggered by the Charge Point proxy interface [ICentralSystem::IChargePoint](./src/ocpp16/centralsystem/interface/ICentralSystem.h) which is instanciated by **Open OCPP** for each connected Charge Point.

Extract of a quick start main() :

```
int main()
{
    // Configuration
    CentralSystemDemoConfig config("config.ini");

    // Event handler
    CentralSystemDemoConfig event_handler(config);

    // Instanciate central system
    std::unique_ptr<ICentralSystem> central_system = ICentralSystem::create(config.stackConfig(), event_handler);
    central_system->start();

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        // Wait for at least 1 connected charge point
        while (event_handler.chargePoints().size() == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // For each connected charge point
        for (auto& iter_chargepoint : event_handler.chargePoints())
        {
            {
                auto chargepoint = iter_chargepoint.second->proxy();

                std::cout << "---------------------------------------------" << std::endl;
                std::cout << "Charge point : " << chargepoint->identifier() << std::endl;
                std::cout << "---------------------------------------------" << std::endl;

                std::cout << "Read whole charge point configuration..." << std::endl;
                std::vector<std::string> keys;
                std::vector<KeyValue>    config_keys;
                std::vector<std::string> unknown_keys;
                if (chargepoint->getConfiguration(keys, config_keys, unknown_keys))
                {
                    std::cout << "Configuration keys :" << std::endl;
                    for (const KeyValue& key_value : config_keys)
                    {
                        std::cout << " - " << key_value.key.str() << " = " << (key_value.value.isSet() ? key_value.value.value().str() : "")
                                  << " " << (key_value.readonly ? "(read-only)" : "") << std::endl;
                    }
                }
                else
                {
                    std::cout << "Failed!" << std::endl;
                }

                std::cout << "Configure heartbeat interval..." << std::endl;
                ConfigurationStatus config_status = chargepoint->changeConfiguration("HeartbeatInterval", "10");
                std::cout << ConfigurationStatusHelper.toString(config_status) << std::endl;

                std::cout << "Trigger status notification..." << std::endl;
                TriggerMessageStatus trigger_status =
                    chargepoint->triggerMessage(MessageTrigger::StatusNotification, Optional<unsigned int>());
                std::cout << TriggerMessageStatusHelper.toString(trigger_status) << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    return 0;
}
```
