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

#ifndef OPENOCPP_ENUMS_H
#define OPENOCPP_ENUMS_H

#include "EnumToStringFromString.h"

namespace ocpp
{
namespace types
{
namespace ocpp16
{

/** @brief Result of registration in response to BootNotification.req */
enum class RegistrationStatus
{
    /** @brief Charge point is accepted by Central System */
    Accepted,
    /** @brief Central System is not yet ready to accept the Charge Point
               Central System may send messages to retrieve information or
               prepare the Charge Point */
    Pending,
    /** @brief Charge point is not accepted by Central System.
               This may happen when the Charge Point id is not known by Central System */
    Rejected
};

/** @brief Helper to convert a RegistrationStatus enum to string */
extern const EnumToStringFromString<RegistrationStatus> RegistrationStatusHelper;

/** @brief Status reported in StatusNotification.req. A status can be reported for the Charge Point main controller
           (connectorId = 0) or for a specific connector. Status for the Charge Point main controller is a subset of the
           enumeration: Available, Unavailable or Faulted.
           States considered Operative are: Available, Preparing, Charging, SuspendedEVSE, SuspendedEV, Finishing, Reserved.
           States considered Inoperative are: Unavailable, Faulted. */
enum class ChargePointStatus
{
    /** @brief When a Connector becomes available for a new user (Operative) */
    Available,
    /** @brief When a Connector becomes no longer available for a new user but there is no ongoing Transaction (yet). Typically a Connector
               is in preparing state when a user presents a tag, inserts a cable or a vehicle occupies the parking bay
               (Operative) */
    Preparing,
    /** @brief Charging When the contactor of a Connector closes, allowing the vehicle to charge
               (Operative) */
    Charging,
    /** @brief When the EV is connected to the EVSE but the EVSE is not offering energy to the EV, e.g. due to a smart charging restriction,
               local supply power constraints, or as the result of StartTransaction.conf indicating that charging is not allowed etc.
               (Operative) */
    SuspendedEVSE,
    /** @brief When the EV is connected to the EVSE and the EVSE is offering energy but the EV is not taking any energy.
               (Operative) */
    SuspendedEV,
    /** @brief When a Transaction has stopped at a Connector, but the Connector is not yet available for a new user, e.g. the cable has not
               been removed or the vehicle has not left the parking bay
               (Operative) */
    Finishing,
    /** @brief When a Connector becomes reserved as a result of a Reserve Now command
               (Operative) */
    Reserved,
    /** @brief When a Connector becomes unavailable as the result of a Change Availability command or an event upon which the Charge
               Point transitions to unavailable at its discretion. Upon receipt of a Change Availability command, the status MAY change
               immediately or the change MAY be scheduled. When scheduled, the Status Notification shall be send when the availability
               change becomes effective
               (Inoperative) */
    Unavailable,
    /** @brief When a Charge Point or connector has reported an error and is not available for energy delivery . (Inoperative) */
    Faulted
};

/** @brief Helper to convert a ChargePointStatus enum to string */
extern const EnumToStringFromString<ChargePointStatus> ChargePointStatusHelper;

/** @brief Charge Point status reported in StatusNotification.req */
enum class ChargePointErrorCode
{
    /** @brief Failure to lock or unlock connector */
    ConnectorLockFailure,
    /** @brief Communication failure with the vehicle, might be Mode 3 or other communication protocol problem. This is
               not a real error in the sense that the Charge Point doesn’t need to go to the faulted state. Instead, it should go
               to the SuspendedEVSE state */
    EVCommunicationError,
    /** @brief Ground fault circuit interrupter has been activated */
    GroundFailure,
    /** @brief Temperature inside Charge Point is too high */
    HighTemperature,
    /** @brief Error in internal hard- or software component */
    InternalError,
    /** @brief The authorization information received from the Central System is in conflict with the LocalAuthorizationList */
    LocalListConflict,
    /** @brief No error to report */
    NoError,
    /** @brief Other type of error. More information in vendorErrorCode */
    OtherError,
    /** @brief Over current protection device has tripped */
    OverCurrentFailure,
    /** @brief Voltage has risen above an acceptable level */
    OverVoltage,
    /** @brief Failure to read electrical/energy/power meter */
    PowerMeterFailure,
    /** @brief Failure to control power switch */
    PowerSwitchFailure,
    /** @brief Failure with idTag reader */
    ReaderFailure,
    /** @brief Unable to perform a reset */
    ResetFailure,
    /** @brief Voltage has dropped below an acceptable level */
    UnderVoltage,
    /** @brief Wireless communication device reports a weak signal */
    WeakSignal
};

/** @brief Helper to convert a ChargePointErrorCode enum to string */
extern const EnumToStringFromString<ChargePointErrorCode> ChargePointErrorCodeHelper;

/** @brief Type of request to be triggered in a TriggerMessage.req */
enum class MessageTrigger
{
    /** @brief To trigger a BootNotification request */
    BootNotification,
    /** @brief To trigger a DiagnosticsStatusNotification request */
    DiagnosticsStatusNotification,
    /** @brief To trigger a FirmwareStatusNotification request */
    FirmwareStatusNotification,
    /** @brief To trigger a Heartbeat request */
    Heartbeat,
    /** @brief To trigger a MeterValues request */
    MeterValues,
    /** @brief To trigger a StatusNotification request */
    StatusNotification
};

/** @brief Helper to convert a MessageTrigger enum to string */
extern const EnumToStringFromString<MessageTrigger> MessageTriggerHelper;

/** @brief Status in TriggerMessage.conf */
enum class TriggerMessageStatus
{
    /** @brief Requested notification will be sent */
    Accepted,
    /** @brief Requested notification will not be sent */
    Rejected,
    /** @brief Requested notification cannot be sent because it is
               either not implemented or unknown */
    NotImplemented
};

/** @brief Helper to convert a TriggerMessageStatus enum to string */
extern const EnumToStringFromString<TriggerMessageStatus> TriggerMessageStatusHelper;

/** @brief Status in a response to an Authorize.req */
enum class AuthorizationStatus
{
    /** @brief Identifier is allowed for charging */
    Accepted,
    /** @brief Identifier has been blocked. Not allowed for charging */
    Blocked,
    /** @brief Identifier has expired. Not allowed for charging */
    Expired,
    /** @brief Identifier is unknown. Not allowed for charging */
    Invalid,
    /** @brief Identifier is already involved in another transaction and multiple transactions are not allowed. (Only relevant for a
               StartTransaction.req */
    ConcurrentTx
};

/** @brief Helper to convert a AuthorizationStatus enum to string */
extern const EnumToStringFromString<AuthorizationStatus> AuthorizationStatusHelper;

/** @brief Status returned in response to ClearCache.req */
enum class ClearCacheStatus
{
    /** @brief Command has been executed */
    Accepted,
    /** @brief Command has not been executed */
    Rejected
};

/** @brief Helper to convert a ClearCacheStatus enum to string */
extern const EnumToStringFromString<ClearCacheStatus> ClearCacheStatusHelper;

/** @brief Status in ChangeConfiguration.conf */
enum class ConfigurationStatus
{
    /** @brief Configuration key is supported and setting has been changed */
    Accepted,
    /** @brief Configuration key is supported, but setting could not be changed */
    Rejected,
    /** @brief Configuration key is supported and setting has been changed, but change will be available after reboot (Charge Point will not
               reboot itself) */
    RebootRequired,
    /** @brief Configuration key is not supported */
    NotSupported
};

/** @brief Helper to convert a ConfigurationStatus enum to string */
extern const EnumToStringFromString<ConfigurationStatus> ConfigurationStatusHelper;

/** @brief Requested availability change in ChangeAvailability.req */
enum class AvailabilityType
{
    /** @brief Charge point is not available for charging */
    Inoperative,
    /** @brief Charge point is available for charging */
    Operative
};

/** @brief Helper to convert a AvailabilityType enum to string */
extern const EnumToStringFromString<AvailabilityType> AvailabilityTypeHelper;

/** @brief Status returned in response to ChangeAvailability.req */
enum class AvailabilityStatus
{
    /** @brief Request has been accepted and will be executed */
    Accepted,
    /** @brief Request has not been accepted and will not be executed */
    Rejected,
    /** @brief Request has been accepted and will be executed when transaction(s) in progress have finished */
    Scheduled
};

/** @brief Helper to convert a AvailabilityStatus enum to string */
extern const EnumToStringFromString<AvailabilityStatus> AvailabilityStatusHelper;

/** @brief Status in ReserveNow.conf */
enum class ReservationStatus
{
    /** @brief Reservation has been made */
    Accepted,
    /** @brief Reservation has not been made because connectors or specified connector are in a faulted state */
    Faulted,
    /** @brief Reservation has not been made. All connectors or the specified connector are occupied */
    Occupied,
    /** @brief Reservation has not been made.Charge Point is not configured to accept reservations */
    Rejected,
    /** @brief Reservation has not been made, because connectors or specified connector are in an unavailable state */
    Unavailable
};

/** @brief Helper to convert a ReservationStatus enum to string */
extern const EnumToStringFromString<ReservationStatus> ReservationStatusHelper;

/** @brief Status in CancelReservation.conf */
enum class CancelReservationStatus
{
    /** @brief Reservation for the identifier has been cancelled */
    Accepted,
    /** @brief Reservation could not be cancelled, because there is no reservation active for the identifier */
    Rejected
};

/** @brief Helper to convert a CancelReservationStatus enum to string */
extern const EnumToStringFromString<CancelReservationStatus> CancelReservationStatusHelper;

/** @brief Status in DataTransfer.conf */
enum class DataTransferStatus
{
    /** @brief Message has been accepted and the contained request is accepted */
    Accepted,
    /** @brief Message has been accepted but the contained request is rejected */
    Rejected,
    /** @brief Message could not be interpreted due to unknown messageId string */
    UnknownMessageId,
    /** @brief Message could not be interpreted due to unknown vendorId string */
    UnknownVendorId
};

/** @brief Helper to convert a DataTransferStatus enum to string */
extern const EnumToStringFromString<DataTransferStatus> DataTransferStatusHelper;

/** @brief Values of the context field of a value in SampledValue */
enum class ReadingContext
{
    /** @brief Value taken at start of interruption */
    InterruptionBegin,
    /** @brief Value taken when resuming after interruption */
    InterruptionEnd,
    /** @brief Value for any other situations */
    Other,
    /** @brief Value taken at clock aligned interval */
    SampleClock,
    /** @brief Value taken as periodic sample relative to start time of transaction */
    SamplePeriodic,
    /** @brief Value taken at start of transaction */
    TransactionBegin,
    /** @brief Value taken at end of transaction */
    TransactionEnd,
    /** @brief Value taken in response to a TriggerMessage.req */
    Trigger
};

/** @brief Helper to convert a ReadingContext enum to string */
extern const EnumToStringFromString<ReadingContext> ReadingContextHelper;

/** @brief Format that specifies how the value element in SampledValue is to be interpreted */
enum class ValueFormat
{
    /** @brief Data is to be interpreted as integer/decimal numeric data */
    Raw,
    /** @brief Data is represented as a signed binary data block, encoded as hex data */
    SignedData
};

/** @brief Helper to convert a ValueFormat enum to string */
extern const EnumToStringFromString<ValueFormat> ValueFormatHelper;

/** @brief Allowable values of the optional "measurand" field of a Value element, as used in MeterValues.req and
           StopTransaction.req messages. Default value of "measurand" is always "Energy.Active.Import.Register" */
enum class Measurand
{
    /** @brief Export Instantaneous current flow from EV */
    Current,
    /** @brief Instantaneous current flow to EV */
    CurrentImport,
    /** @brief Maximum current offered to EV */
    CurrentOffered,
    /** @brief Numerical value read from the "active electrical energy"(Wh or kWh) register of
               the(most authoritative) electrical meter measuring energy exported(to the grid) */
    EnergyActiveExportRegister,
    /** @brief Numerical value read from the "active electrical energy"(Wh or kWh) register of
               the(most authoritative) electrical meter measuring energy
               imported(from the grid supply) */
    EnergyActiveImportRegister,
    /** @brief Numerical value read from the "reactive electrical energy"(VARh or kVARh) register of
               the(most authoritative) electrical meter measuring energy exported(to the grid) */
    EnergyReactiveExportRegister,
    /** @brief Numerical value read from the "reactive electrical energy"(VARh or kVARh) register of
        the(most authoritative) electrical meter measuring energy imported(from the grid supply) */
    EnergyReactiveImportRegister,
    /** @brief Absolute amount of "active electrical energy"(Wh or kWh) exported(to the grid) during an associated time
               "interval", specified by a Metervalues ReadingContext, and applicable interval duration configuration values
               (in seconds) for "ClockAlignedDataInterval" and "MeterValueSampleInterval" */
    EnergyActiveExportInterval,
    /** @brief Absolute amount of "active electrical energy"(Wh or kWh)
               imported(from the grid supply) during an associated time "interval",
               specified by a Metervalues ReadingContext, and applicable interval duration
               configuration values (in seconds) for "ClockAlignedDataInterval" and "MeterValueSampleInterval" */
    EnergyActiveImportInterval,
    /** @brief Absolute amount of "reactive electrical energy"(VARh or kVARh)
               exported(to the grid) during an associated time "interval",
               specified by a Metervalues ReadingContext, and applicable interval duration configuration
               values (in seconds) for "ClockAlignedDataInterval" and "MeterValueSampleInterval" */
    EnergyReactiveExportInterval,
    /** @brief Absolute amount of "reactive electrical energy"(VARh or kVARh)
               imported(from the grid supply) during an associated time "interval",
               specified by a Metervalues ReadingContext, and applicable interval duration
               configuration values (in seconds) for "ClockAlignedDataInterval" and "MeterValueSampleInterval" */
    EnergyReactiveImportInterval,
    /** @brief Instantaneous reading of powerline frequency. NOTE: OCPP 1.6 does not have a UnitOfMeasure for
               frequency, the UnitOfMeasure for any SampledValue with measurand: Frequency is Hertz */
    Frequency,
    /** @brief Instantaneous active power exported by EV.(W or kW) */
    PowerActiveExport,
    /** @brief Instantaneous active power imported by EV.(W or kW) */
    PowerActiveImport,
    /** @brief Instantaneous power factor of total energy flow */
    PowerFactor,
    /** @brief Maximum power offered to EV */
    PowerOffered,
    /** @brief Instantaneous reactive power exported by EV.(var or kvar) */
    PowerReactiveExport,
    /** @brief Instantaneous reactive power imported by EV.(var or kvar) */
    PowerReactiveImport,
    /** @brief Fan speed in RPM */
    RPM,
    /** @brief State of charge of charging vehicle in percentage */
    SoC,
    /** @brief Temperature reading inside Charge Point */
    Temperature,
    /** @brief Instantaneous AC RMS supply voltage */
    Voltage
};

/** @brief Helper to convert a Measurand enum to string */
extern const EnumToStringFromString<Measurand> MeasurandHelper;

/** @brief Phase as used in SampledValue. Phase specifies how a measured value is to be interpreted.
           Please note that not all values of Phase are applicable to all Measurands */
enum class Phase
{
    /** @brief Measured on L1 */
    L1,
    /** @brief Measured on L2 */
    L2,
    /** @brief Measured on L3 */
    L3,
    /** @brief Measured on Neutral */
    N,
    /** @brief Measured on L1 with respect to Neutral conductor */
    L1N,
    /** @brief Measured on L2 with respect to Neutral conductor */
    L2N,
    /** @brief Measured on L3 with respect to Neutral conductor */
    L3N,
    /** @brief Measured between L1 and L2 */
    L1L2,
    /** @brief Measured between L2 and L3 */
    L2L3,
    /** @brief Measured between L3 and L1 */
    L3L1
};

/** @brief Helper to convert a Phase enum to string */
extern const EnumToStringFromString<Phase> PhaseHelper;

/** @brief Allowable values of the optional "location" field of a value element in SampledValue */
enum class Location
{
    /** @brief Measurement inside body of Charge Point (e.g. Temperature) */
    Body,
    /** @brief Measurement taken from cable between EV and Charge Point */
    Cable,
    /** @brief Measurement taken by EV */
    EV,
    /** @brief Measurement at network(“grid”) inlet connection */
    Inlet,
    /** @brief Measurement at a Connector.Default value */
    Outlet
};

/** @brief Helper to convert a Location enum to string */
extern const EnumToStringFromString<Location> LocationHelper;

/** @brief Allowable values of the optional "unit" field of a Value element, as used in SampledValue. Default value of "unit"
           is always "Wh" */
enum class UnitOfMeasure
{
    /** @brief Watt-hours (energy). Default */
    Wh,
    /** @brief kiloWatt-hours (energy) */
    kWh,
    /** @brief Var-hours (reactive energy) */
    varh,
    /** @brief kilovar-hours (reactive energy) */
    kvarh,
    /** @brief Watts (power) */
    W,
    /** @brief kilowatts (power) */
    kW,
    /** @brief VoltAmpere (apparent power) */
    VA,
    /** @brief kiloVolt Ampere (apparent power) */
    kVA,
    /** @brief Vars (reactive power) */
    var,
    /** @brief kilovars (reactive power) */
    kvar,
    /** @brief Amperes (current) */
    A,
    /** @brief Voltage (r.m.s. AC) */
    V,
    /** @brief Degrees (temperature) */
    Celsius,
    /** @brief Degrees (temperature) */
    Fahrenheit,
    /** @brief Degrees Kelvin (temperature) */
    K,
    /** @brief Percentage */
    Percent
};

/** @brief Helper to convert a UnitOfMeasure enum to string */
extern const EnumToStringFromString<UnitOfMeasure> UnitOfMeasureHelper;

/** @brief Purpose of the charging profile, as used in: ChargingProfile */
enum class ChargingProfilePurposeType
{
    /** @brief Configuration for the maximum power or current available for an entire Charge Point */
    ChargePointMaxProfile,
    /** @brief Default profile *that can be configured in the Charge Point.
               When a new transaction is started, this profile SHALL be used,
               unless it was a transaction that was started by a RemoteStartTransaction.req with a
               ChargeProfile that is accepted by the Charge Point */
    TxDefaultProfile,
    /** @brief Profile with constraints to be imposed by the Charge Point on the current transaction,
               or on a new transaction when this is started via a RemoteStartTransaction.req with a
               ChargeProfile. A profile with this purpose SHALL cease to be valid when the transaction terminates */
    TxProfile
};

/** @brief Helper to convert a ChargingProfilePurposeType enum to string */
extern const EnumToStringFromString<ChargingProfilePurposeType> ChargingProfilePurposeTypeHelper;

/** @brief Status returned in response to ClearChargingProfile.req */
enum class ClearChargingProfileStatus
{
    /** @brief Request has been accepted and will be executed */
    Accepted,
    /** @brief No Charging Profile(s) were found matching the request */
    Unknown
};

/** @brief Helper to convert a ClearChargingProfileStatus enum to string */
extern const EnumToStringFromString<ClearChargingProfileStatus> ClearChargingProfileStatusHelper;

/** @brief Status returned in response to SetChargingProfile.req */
enum class ChargingProfileStatus
{
    /** @brief Request has been accepted and will be executed */
    Accepted,
    /** @brief Request has not been accepted and will not be executed */
    Rejected,
    /** @brief Charge Point indicates that the request is not supported */
    NotSupported
};

/** @brief Helper to convert a ChargingProfileStatus enum to string */
extern const EnumToStringFromString<ChargingProfileStatus> ChargingProfileStatusHelper;

/** @brief Kind of charging profile, as used in: ChargingProfile */
enum class ChargingProfileKindType
{
    /** @brief Schedule periods are relative to a fixed point in time defined in the schedule */
    Absolute,
    /** @brief The schedule restarts periodically at the first schedule period */
    Recurring,
    /** @brief Schedule periods are relative to a situation-specific start point (such as the start of a Transaction)
               that is determined by the charge point */
    Relative
};

/** @brief Helper to convert a ChargingProfileKindType enum to string */
extern const EnumToStringFromString<ChargingProfileKindType> ChargingProfileKindTypeHelper;

/** @brief Type of recurrence of a charging profile, as used in ChargingProfile */
enum class RecurrencyKindType
{
    /** @brief The schedule restarts every 24 hours, at the same time as in the startSchedule */
    Daily,
    /** @brief The schedule restarts every 7 days, at the same time and day-of-the-week as in the startSchedule */
    Weekly
};

/** @brief Helper to convert a RecurrencyKindType enum to string */
extern const EnumToStringFromString<RecurrencyKindType> RecurrencyKindTypeHelper;

/** @brief Unit in which a charging schedule is defined, as used in: GetCompositeSchedule.req and ChargingSchedule */
enum class ChargingRateUnitType
{
    /** @brief Watts (power).
               This is the TOTAL allowed charging power.
               If used for AC Charging, the phase current should be calculated via: Current per phase = Power / (Line Voltage * Number of
               Phases). The "Line Voltage" used in the calculation is not the measured voltage, but the set voltage for the area (hence, 230 of
               110 volt). The "Number of Phases" is the numberPhases from the ChargingSchedulePeriod.
               It is usually more convenient to use this for DC charging.
               Note that if numberPhases in a ChargingSchedulePeriod is absent, 3 SHALL be assumed */
    W,
    /** @brief Amperes (current).
               The amount of Ampere per phase, not the sum of all phases.
               It is usually more convenient to use this for AC charging. */
    A
};

/** @brief Helper to convert a ChargingRateUnitType enum to string */
extern const EnumToStringFromString<ChargingRateUnitType> ChargingRateUnitTypeHelper;

/** @brief Reason for stopping a transaction in StopTransaction.req */
enum class Reason
{
    /** @brief The transaction was stopped because of the authorization status in a StartTransaction.conf */
    DeAuthorized,
    /** @brief Emergency stop button was used */
    EmergencyStop,
    /** @brief Disconnecting of cable, vehicle moved away from inductive charge unit */
    EVDisconnected,
    /** @brief A hard reset command was received */
    HardReset,
    /** @brief Stopped locally on request of the user at the Charge Point. This is a regular termination of a transaction. Examples: presenting
               an RFID tag, pressing a button to stop */
    Local,
    /** @brief Any other reason */
    Other,
    /** @brief Complete loss of power */
    PowerLoss,
    /** @brief A locally initiated reset/reboot occurred. (for instance watchdog kicked in) */
    Reboot,
    /** @brief Stopped remotely on request of the user. This is a regular termination of a transaction. Examples: termination using a
               smartphone app, exceeding a (non local) prepaid credit */
    Remote,
    /** @brief A soft reset command was received */
    SoftReset,
    /** @brief Central System sent an Unlock Connector command */
    UnlockCommand
};

/** @brief Helper to convert a Reason enum to string */
extern const EnumToStringFromString<Reason> ReasonHelper;

/** @brief The result of a RemoteStartTransaction.req or RemoteStopTransaction.req request */
enum class RemoteStartStopStatus
{
    /** @brief Command will be executed */
    Accepted,
    /** @brief Command will not be executed */
    Rejected
};

/** @brief Helper to convert a RemoteStartStopStatus enum to string */
extern const EnumToStringFromString<RemoteStartStopStatus> RemoteStartStopStatusHelper;

/** @brief This contains the field definition of the DiagnosticsStatusNotification.req PDU sent by the Charge Point to the
           Central System */
enum class DiagnosticsStatus
{
    /** @brief Charge Point is not performing diagnostics related tasks. Status Idle SHALL only be used as in a
               DiagnosticsStatusNotification.req that was triggered by a TriggerMessage.req */
    Idle,
    /** @brief Diagnostics information has been uploaded */
    Uploaded,
    /** @brief Uploading of diagnostics failed */
    UploadFailed,
    /** @brief File is being uploaded */
    Uploading
};

/** @brief Helper to convert a DiagnosticsStatus enum to string */
extern const EnumToStringFromString<DiagnosticsStatus> DiagnosticsStatusHelper;

/** @brief Type of reset requested by Reset.req */
enum class ResetType
{
    /** @brief Restart (all) the hardware, the Charge Point is not required to gracefully stop ongoing transaction. If possible the Charge Point
               sends a StopTransaction.req for previously ongoing transactions after having restarted and having been accepted by the
               Central System via a BootNotification.conf. This is a last resort solution for a not correctly functioning Charge Point, by sending
               a "hard" reset, (queued) information might get lost */
    Hard,
    /** @brief Stop ongoing transactions gracefully and sending StopTransaction.req for every ongoing transaction. It should then restart the
               application software (if possible, otherwise restart the processor/controller) */
    Soft
};

/** @brief Helper to convert a ResetType enum to string */
extern const EnumToStringFromString<ResetType> ResetTypeHelper;

/** @brief Result of Reset.req */
enum class ResetStatus
{
    /** @brief Command will be executed */
    Accepted,
    /** @brief Command will not be executed */
    Rejected
};

/** @brief Helper to convert a ResetStatus enum to string */
extern const EnumToStringFromString<ResetStatus> ResetStatusHelper;

/** @brief Status in response to UnlockConnector.req */
enum class UnlockStatus
{
    /** @brief Connector has successfully been unlocked */
    Unlocked,
    /** @brief Failed to unlock the connector: The Charge Point has tried to unlock the connector and has detected that the connector is still
               locked or the unlock mechanism failed */
    UnlockFailed,
    /** @brief Charge Point has no connector lock, or ConnectorId is unknown */
    NotSupported
};

/** @brief Helper to convert a UnlockStatus enum to string */
extern const EnumToStringFromString<UnlockStatus> UnlockStatusHelper;

/** @brief Status of a firmware download as reported in FirmwareStatusNotification.req */
enum class FirmwareStatus
{
    /** @brief New firmware has been downloaded by Charge Point */
    Downloaded,
    /** @brief Charge point failed to download firmware */
    DownloadFailed,
    /** @brief Firmware is being downloaded */
    Downloading,
    /** @brief Charge Point is not performing firmware update related tasks. Status Idle SHALL only be used as in a
           FirmwareStatusNotification.req that was triggered by a TriggerMessage.req */
    Idle,
    /** @brief Installation of new firmware has failed */
    InstallationFailed,
    /** @brief Firmware is being installed */
    Installing,
    /** @brief New firmware has successfully been installed in charge point */
    Installed
};

/** @brief Helper to convert a FirmwareStatus enum to string */
extern const EnumToStringFromString<FirmwareStatus> FirmwareStatusHelper;

/** @brief Type of update for a SendLocalList.req */
enum class UpdateType
{
    /** @brief Indicates that the current Local Authorization List must be updated with the values in this message */
    Differential,
    /** @brief Indicates that the current Local Authorization List must be replaced by the values in this message */
    Full
};

/** @brief Helper to convert a UpdateType enum to string */
extern const EnumToStringFromString<UpdateType> UpdateTypeHelper;

/** @brief Status in response to SendLocalList.req */
enum class UpdateStatus
{
    /** @brief Local Authorization List successfully updated */
    Accepted,
    /** @brief Failed to update the Local Authorization List */
    Failed,
    /** @brief Update of Local Authorization List is not supported by Charge Point */
    NotSupported,
    /** @brief Version number in the request for a differential update is less or equal then version number of current list */
    VersionMismatch
};

/** @brief Helper to convert a UpdateStatus enum to string */
extern const EnumToStringFromString<UpdateStatus> UpdateStatusHelper;

/** @brief Status returned in response to GetCompositeSchedule.req */
enum class GetCompositeScheduleStatus
{
    /** @brief Request has been accepted and will be executed */
    Accepted,
    /** @brief Request has not been accepted and will not be executed */
    Rejected
};

/** @brief Helper to convert a GetCompositeScheduleStatus enum to string */
extern const EnumToStringFromString<GetCompositeScheduleStatus> GetCompositeScheduleStatusHelper;

/** @brief CertificateSignedStatusEnumType is used by: CertificateSigned.conf */
enum class CertificateSignedStatusEnumType
{
    /** @brief Signed certificate is valid */
    Accepted,
    /** @brief Signed certificate is invalid */
    Rejected
};

/** @brief Helper to convert a CertificateSignedStatusEnumType enum to string */
extern const EnumToStringFromString<CertificateSignedStatusEnumType> CertificateSignedStatusEnumTypeHelper;

/** @brief DeleteCertificateStatusEnumType is used by: DeleteCertificate.conf */
enum class DeleteCertificateStatusEnumType
{
    /** @brief Normal successful completion(no errors) */
    Accepted,
    /** @brief Processing failure */
    Failed,
    /** @brief Requested resource not found */
    NotFound
};

/** @brief Helper to convert a DeleteCertificateStatusEnumType enum to string */
extern const EnumToStringFromString<DeleteCertificateStatusEnumType> DeleteCertificateStatusEnumTypeHelper;

/** @brief HashAlgorithmEnumType is used by: CertificateHashDataType */
enum class HashAlgorithmEnumType
{
    /** @brief SHA-256 hash algorithm */
    SHA256,
    /** @brief SHA-384 hash algorithm */
    SHA384,
    /** @brief SHA-512 hash algorithm */
    SHA512
};

/** @brief Helper to convert a HashAlgorithmEnumType enum to string */
extern const EnumToStringFromString<HashAlgorithmEnumType> HashAlgorithmEnumTypeHelper;

/** @brief Type of request to be triggered in a TriggerMessage.req */
enum class MessageTriggerEnumType
{
    /** @brief To trigger a BootNotification.req */
    BootNotification,
    /** @brief To trigger LogStatusNotification.req */
    LogStatusNotification,
    /** @brief To trigger SignedFirmwareStatusNotification.req */
    FirmwareStatusNotification,
    /** @brief To trigger Heartbeat.req */
    Heartbeat,
    /** @brief To trigger MeterValues.req */
    MeterValues,
    /** @brief To trigger a SignCertificate.req with certificateType: ChargePointCertificate */
    SignChargePointCertificate,
    /** @brief To trigger SatusNotification.req */
    StatusNotification
};

/** @brief Helper to convert a MessageTriggerEnumType enum to string */
extern const EnumToStringFromString<MessageTriggerEnumType> MessageTriggerEnumTypeHelper;

/** @brief TriggerMessageStatusEnumType is used by: ExtendedTriggerMessage.conf */
enum class TriggerMessageStatusEnumType
{
    /** @brief Requested message will be sent */
    Accepted,
    /** @brief Requested message will not be sent */
    Rejected,
    /** @brief Requested message cannot be sent because it is either not implemented or unknown */
    NotImplemented
};

/** @brief Helper to convert a TriggerMessageStatusEnumType enum to string */
extern const EnumToStringFromString<TriggerMessageStatusEnumType> TriggerMessageStatusEnumTypeHelper;

/** @brief CertificateUseEnumType is used by: GetInstalledCertificateIds.req, InstallCertificate.req */
enum class CertificateUseEnumType
{
    /** @brief Root certificate, used by the CA to sign the Central System and Charge Point certificate */
    CentralSystemRootCertificate,
    /** @brief Root certificate for verification of the Manufacturer certificate */
    ManufacturerRootCertificate
};

/** @brief Helper to convert a CertificateUseEnumType enum to string */
extern const EnumToStringFromString<CertificateUseEnumType> CertificateUseEnumTypeHelper;

/** @brief GetInstalledCertificateStatusEnumType is used by: GetInstalledCertificateIds.conf */
enum class GetInstalledCertificateStatusEnumType
{
    /** @brief Normal successful completion (no errors) */
    Accepted,
    /** @brief Requested certificate not found */
    NotFound
};

/** @brief Helper to convert a GetInstalledCertificateStatusEnumType enum to string */
extern const EnumToStringFromString<GetInstalledCertificateStatusEnumType> GetInstalledCertificateStatusEnumTypeHelper;

/** @brief LogEnumType is used by: GetLog.req */
enum class LogEnumType
{
    /** @brief This contains the field definition of a diagnostics log file */
    DiagnosticsLog,
    /** @brief Sent by the Central System to the Charge Point to request that the Charge Point uploads the security log */
    SecurityLog
};

/** @brief Helper to convert a LogEnumType enum to string */
extern const EnumToStringFromString<LogEnumType> LogEnumTypeHelper;

/** @brief LogStatusEnumType is used by: GetLog.conf */
enum class LogStatusEnumType
{
    /** @brief Accepted this log upload. This does not mean the log file is uploaded is successfully, the Charge Point will now start the log file
               upload */
    Accepted,
    /** @brief Log update request rejected */
    Rejected,
    /** @brief Accepted this log upload, but in doing this has canceled an ongoing log file upload */
    AcceptedCanceled
};

/** @brief Helper to convert a LogStatusEnumType enum to string */
extern const EnumToStringFromString<LogStatusEnumType> LogStatusEnumTypeHelper;

/** @brief Status of the certificate.
           CertificateStatusEnumType is used by: InstallCertificate.conf */
enum class CertificateStatusEnumType
{
    /** @brief The installation of the certificate succeeded */
    Accepted,
    /** @brief The certificate is valid and correct, but there is another reason the installation did not succeed */
    Failed,
    /** @brief The certificate is invalid and/or incorrect OR the CPO tries to install more certificates than allowed */
    Rejected
};

/** @brief Helper to convert a CertificateStatusEnumType enum to string */
extern const EnumToStringFromString<CertificateStatusEnumType> CertificateStatusEnumTypeHelper;

/** @brief UploadLogStatusEnumType is used by: LogStatusNotification.req */
enum class UploadLogStatusEnumType
{
    /** @brief A badly formatted packet or other protocol incompatibility was detected */
    BadMessage,
    /** @brief The Charge Point is not uploading a log file. Idle SHALL only be used when the message was triggered by a
               ExtendedTriggerMessage.req */
    Idle,
    /** @brief The server does not support the operation */
    NotSupportedOperation,
    /** @brief Insufficient permissions to perform the operation */
    PermissionDenied,
    /** @brief File has been uploaded successfully */
    Uploaded,
    /** @brief Failed to upload the requested file */
    UploadFailure,
    /** @brief File is being uploaded */
    Uploading
};

/** @brief Helper to convert a UploadLogStatusEnumType enum to string */
extern const EnumToStringFromString<UploadLogStatusEnumType> UploadLogStatusEnumTypeHelper;

/** @brief Generic message response status */
enum class GenericStatusEnumType
{
    /** @brief Request has been accepted and will be executed */
    Accepted,
    /** @brief Request has not been accepted and will not be executed */
    Rejected
};

/** @brief Helper to convert a GenericStatusEnumType enum to string */
extern const EnumToStringFromString<GenericStatusEnumType> GenericStatusEnumTypeHelper;

/** @brief Status of a firmware download.
           A value with "Intermediate state" in the description, is an intermediate state, update process is not finished.
           A value with "Failure end state" in the description, is an end state, update process has stopped, update failed.
           A value with "Successful end state" in the description, is an end state, update process has stopped, update
           successful.
           FirmwareStatusEnumType is used by: SignedFirmwareStatusNotification.req */
enum class FirmwareStatusEnumType
{
    /** @brief Intermediate state. New firmware has been downloaded by Charge Point */
    Downloaded,
    /** @brief Failure end state. Charge Point failed to download firmware */
    DownloadFailed,
    /** @brief Intermediate state. Firmware is being downloaded */
    Downloading,
    /** @brief Intermediate state. Downloading of new firmware has been scheduled */
    DownloadScheduled,
    /** @brief Intermediate state. Downloading has been paused */
    DownloadPaused,
    /** @brief Charge Point is not performing firmware update related tasks. Status Idle SHALL only be used as in a
               SignedFirmwareStatusNotification.req that was triggered by ExtendedTriggerMessage.req */
    Idle,
    /** @brief Failure end state. Installation of new firmware has failed */
    InstallationFailed,
    /** @brief Intermediate state. Firmware is being installed */
    Installing,
    /** @brief  Successful end state. New firmware has successfully been installed in Charge Point */
    Installed,
    /** @brief Intermediate state. Charge Point is about to reboot to activate new firmware. This status MAY be omitted if a
               reboot is an integral part of the installation and cannot be reported separately */
    InstallRebooting,
    /** @brief Intermediate state. Installation of the downloaded firmware is scheduled to take place on installDateTime given
               in SignedUpdateFirmware.req */
    InstallScheduled,
    /** @brief  Failure end state. Verification of the new firmware (e.g. using a checksum or some other means) has failed and
                installation will not proceed. (Final failure state) */
    InstallVerificationFailed,
    /** @brief Failure end state. The firmware signature is not valid */
    InvalidSignature,
    /** @brief Intermediate state. Provide signature successfully verified */
    SignatureVerified
};

/** @brief Helper to convert a FirmwareStatusEnumType enum to string */
extern const EnumToStringFromString<FirmwareStatusEnumType> FirmwareStatusEnumTypeHelper;

/** @brief UpdateFirmwareStatusEnumType is used by: SignedUpdateFirmware.conf */
enum class UpdateFirmwareStatusEnumType
{
    /** @brief Accepted this firmware update request. This does not mean the firmware update is successful, the Charge Point will now start
               the firmware update process */
    Accepted,
    /** @brief Firmware update request rejected */
    Rejected,
    /** @brief Accepted this firmware update request, but in doing this has canceled an ongoing firmware update */
    AcceptedCanceled,
    /** @brief The certificate is invalid */
    InvalidCertificate,
    /** @brief Failure end state. The Firmware Signing certificate has been revoked */
    RevokedCertificate
};

/** @brief Helper to convert a UpdateFirmwareStatusEnumType enum to string */
extern const EnumToStringFromString<UpdateFirmwareStatusEnumType> UpdateFirmwareStatusEnumTypeHelper;

/** @brief Status of the EV Contract certificate */
enum class AuthorizeCertificateStatusEnumType
{
    /** @brief Positive response */
    Accepted,
    /** @brief Identifier has been blocked. Not allowed for charging */
    SignatureError,
    /** @brief If the OEMProvisioningCert in the CertificateInstallationReq, the Contract Certificate in the CertificateUpdateReq, or the
               ContractCertificate in the PaymentDetailsReq is expired */
    CertificateExpired,
    /** @brief Used when the SECC or Central System matches the ContractCertificate contained in a CertificateUpdateReq or PaymentDetailsReq
               with a CRL and the Contract Certificate is marked as revoked, OR when the SECC or Central System matches the OEM Provisioning
               Certificate contained in a CertificateInstallationReq with a CRL and the OEM Provisioning Certificate is marked as revoked.
               The revocation status can alternatively be obtained through an OCSP responder */
    CertificateRevoked,
    /** @brief If the new certificate cannot be retrieved from secondary actor within the specified timeout */
    NoCertificateAvailable,
    /** @brief If the ContractSignatureCertChain contained in the CertificateInstallationReq message is not valid */
    CertChainError,
    /** @brief If the EMAID provided by EVCC during CertificateUpdateReq is not accepted by secondary actor */
    ContractCancelled
};

/** @brief Helper to convert a AuthorizeCertificateStatusEnumType enum to string */
extern const EnumToStringFromString<AuthorizeCertificateStatusEnumType> AuthorizeCertificateStatusEnumTypeHelper;

/** @brief Action to apply on a certificate */
enum class CertificateActionEnumType
{
    /** @brief Install the provided certificate */
    Install,
    /** @brief Update the provided certificate */
    Update
};

/** @brief Helper to convert a CertificateActionEnumType enum to string */
extern const EnumToStringFromString<CertificateActionEnumType> CertificateActionEnumTypeHelper;

/** @brief Usage of a certificate */
enum class GetCertificateIdUseEnumType
{
    /** @brief Use for certificate of the V2G Root */
    V2GRootCertificate,
    /** @brief Use for certificate from an eMobility Service provider. To support PnC charging with contracts from service providers that not derived
               their certificates from the V2G root */
    MORootCertificate,
    /** @brief ISO 15118 V2G certificate chain (excluding the V2GRootCertificate) */
    V2GCertificateChain,
    /** @brief ISO 15118-20 OEM root certificates */
    OEMRootCertificate
};

/** @brief Helper to convert a GetCertificateIdUseEnumType enum to string */
extern const EnumToStringFromString<GetCertificateIdUseEnumType> GetCertificateIdUseEnumTypeHelper;

/** @brief Status of a certificate */
enum class GetCertificateStatusEnumType
{
    /** @brief Successfully retrieved the OCSP certificate status */
    Accepted,
    /** @brief Failed to retrieve the OCSP certificate status */
    Failed
};

/** @brief Helper to convert a GetCertificateStatusEnumType enum to string */
extern const EnumToStringFromString<GetCertificateStatusEnumType> GetCertificateStatusEnumTypeHelper;

/** @brief Status of a certificate installation */
enum class InstallCertificateStatusEnumType
{
    /** @brief The installation of the certificate succeeded */
    Accepted,
    /** @brief The certificate is invalid and/or incorrect OR the CSO tries to install more certificates than allowed */
    Rejected,
    /** @brief The certificate is valid and correct, but there is another reason the installation did not succeed */
    Failed
};

/** @brief Helper to convert a InstallCertificateStatusEnumType enum to string */
extern const EnumToStringFromString<InstallCertificateStatusEnumType> InstallCertificateStatusEnumTypeHelper;

/** @brief Usage of a certificate to install */
enum class InstallCertificateUseEnumType
{
    /** @brief Use for certificate of the V2G Root, a V2G Charge Point Certificate MUST be derived from one of the installed V2GRootCertificate
               certificates */
    V2GRootCertificate,
    /** @brief Use for certificate from an eMobility Service */
    MORootCertificate,
    /** @brief Use for certificate from an OEM (Vehicle Manufacturer used for bi-directional TLS connection between Secc and EV */
    OEMRootCertificate
};

/** @brief Helper to convert a InstallCertificateUseEnumType enum to string */
extern const EnumToStringFromString<InstallCertificateUseEnumType> InstallCertificateUseEnumTypeHelper;

/** @brief Iso15118EVCertificateStatusEnumType is used by: get15118EVCertificate:Get15118EVCertificate.conf */
enum class Iso15118EVCertificateStatusEnumType
{
    /** @brief exiResponse included. This is no indication whether the update was successful, just that the message was processed properly */
    Accepted,
    /** @brief Processing of the message was not successful, no exiResponse included */
    Failed
};

/** @brief Helper to convert a Iso15118EVCertificateStatusEnumType enum to string */
extern const EnumToStringFromString<Iso15118EVCertificateStatusEnumType> Iso15118EVCertificateStatusEnumTypeHelper;

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_ENUMS_H
