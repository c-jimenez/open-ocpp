######################################################
#               OCPP messages library                #
######################################################


# Library target
add_library(messages STATIC
    MessageDispatcher.cpp
    MessagesConverter.cpp

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
    GetCompositeSchedule.cpp
    GetDiagnostics.cpp
    GetInstalledCertificateIds.cpp
    GetLocalListVersion.cpp
    GetLog.cpp
    Heartbeat.cpp
    InstallCertificate.cpp
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
    types/CertificateHashDataTypeConverter.cpp
    types/ChargingProfileConverter.cpp
    types/ChargingScheduleConverter.cpp
    types/IdTagInfoConverter.cpp
    types/MeterValueConverter.cpp
)

# Exported includes
target_include_directories(messages PUBLIC . types)

# Dependencies
target_link_libraries(messages
    rpc
    json
    helpers
    types
    log
    -lstdc++fs
)
