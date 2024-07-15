######################################################
#            OCPP 1.6 messages library               #
######################################################


# Library target
add_library(messages16 OBJECT
    MessagesConverter.cpp
    MessagesValidator.cpp

    Authorize.cpp
    BootNotification.cpp
    CancelReservation.cpp
    CertificateSigned.cpp
    ChangeAvailability.cpp
    ChangeConfiguration.cpp
    ClearCache.cpp
    ClearChargingProfile.cpp
    DataTransfer.cpp
    DeleteCertificate.cpp
    DiagnosticsStatusNotification.cpp
    ExtendedTriggerMessage.cpp
    FirmwareStatusNotification.cpp
    GetConfiguration.cpp
    Get15118EVCertificate.cpp
    GetCertificateStatus.cpp
    GetCompositeSchedule.cpp
    GetDiagnostics.cpp
    GetInstalledCertificateIds.cpp
    GetLocalListVersion.cpp
    GetLog.cpp
    Heartbeat.cpp
    InstallCertificate.cpp
    Iso15118Authorize.cpp
    Iso15118GetInstalledCertificateIds.cpp
    Iso15118InstallCertificate.cpp
    Iso15118TriggerMessage.cpp
    LogStatusNotification.cpp
    MeterValues.cpp
    RemoteStartTransaction.cpp
    RemoteStopTransaction.cpp
    ReserveNow.cpp
    Reset.cpp
    SecurityEventNotification.cpp
    SendLocalList.cpp
    SetChargingProfile.cpp
    SignCertificate.cpp
    SignedFirmwareStatusNotification.cpp
    SignedUpdateFirmware.cpp
    StartTransaction.cpp
    StatusNotification.cpp
    StopTransaction.cpp
    TriggerMessage.cpp
    UnlockConnector.cpp
    UpdateFirmware.cpp

    types/AuthorizationDataConverter.cpp
    types/CertificateHashDataChainTypeConverter.cpp
    types/CertificateHashDataTypeConverter.cpp
    types/ChargingProfileConverter.cpp
    types/ChargingScheduleConverter.cpp
    types/IdTagInfoConverter.cpp
    types/IdTokenInfoTypeConverter.cpp
    types/MeterValueConverter.cpp
    types/OcspRequestDataTypeConverter.cpp
)

# Exported includes
target_include_directories(messages16 PUBLIC . types)

# Dependencies
target_link_libraries(messages16 PUBLIC types16 messages)
