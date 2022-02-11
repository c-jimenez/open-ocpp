######################################################
#            OCPP chargepoint library                #
######################################################


# Library target
add_library(chargepoint OBJECT
    ChargePoint.cpp
    authent/AuthentCache.cpp
    authent/AuthentLocalList.cpp
    authent/AuthentManager.cpp
    config/ConfigManager.cpp
    connector/Connectors.cpp
    datatransfer/DataTransferManager.cpp
    maintenance/MaintenanceManager.cpp
    metervalues/MeterValuesManager.cpp
    requestfifo/RequestFifo.cpp
    requestfifo/RequestFifoManager.cpp
    reservation/ReservationManager.cpp
    security/CaCertificatesDatabase.cpp
    security/CpCertificatesDatabase.cpp
    security/SecurityLogsDatabase.cpp
    security/SecurityManager.cpp
    smartcharging/ProfileDatabase.cpp
    smartcharging/SmartChargingManager.cpp
    status/StatusManager.cpp
    transaction/TransactionManager.cpp
    trigger/TriggerMessageManager.cpp
)

# Exported includes
target_include_directories(chargepoint PUBLIC interface)

# Private includes
target_include_directories(chargepoint PRIVATE authent
                                               config
                                               connector
                                               datatransfer
                                               interface
                                               maintenance
                                               metervalues
                                               requestfifo
                                               reservation
                                               security
                                               smartcharging
                                               status
                                               transaction
                                               trigger)

# Dependencies
target_link_libraries(chargepoint PUBLIC
    config
    database
    messages
    rpc
    helpers
    log
    version
    x509
)
