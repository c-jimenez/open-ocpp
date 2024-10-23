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

#ifndef OPENOCPP_ICENTRALSYSTEM_H
#define OPENOCPP_ICENTRALSYSTEM_H

#include "AuthorizationData.h"
#include "Certificate.h"
#include "CertificateHashDataChainType.h"
#include "CertificateHashDataType.h"
#include "ChargingProfile.h"
#include "ICentralSystemConfig.h"
#include "IChargePointRequestHandler.h"
#include "KeyValue.h"
#include "SecurityEvent.h"

#include <memory>

namespace ocpp
{
namespace helpers
{
class ITimerPool;
class WorkerThreadPool;
} // namespace helpers
namespace database
{
class Database;
} // namespace database

namespace centralsystem
{

class ICentralSystemEventsHandler;

/** @brief Interface for central system implementations */
class ICentralSystem
{
  public:
    /**
     * @brief Instanciate a central system
     * @param stack_config Stack configuration
     * @param event_handler Stack event handler
     */
    static std::unique_ptr<ICentralSystem> create(const ocpp::config::ICentralSystemConfig& stack_config,
                                                  ICentralSystemEventsHandler&              events_handler);

    /**
     * @brief Instanciate a central system with the provided timer and worker pools
     *        To use when you have to instanciate multiple Central System / Charge Point
     *        => Allow to reduce thread and memory usage
     * @param stack_config Stack configuration
     * @param event_handler Stack event handler
     * @param timer_pool Timer pool
     * @param worker_pool Worker thread pool
     */
    static std::unique_ptr<ICentralSystem> create(const ocpp::config::ICentralSystemConfig&        stack_config,
                                                  ICentralSystemEventsHandler&                     events_handler,
                                                  std::shared_ptr<ocpp::helpers::ITimerPool>       timer_pool,
                                                  std::shared_ptr<ocpp::helpers::WorkerThreadPool> worker_pool);

    /** @brief Destructor */
    virtual ~ICentralSystem() { }

    /**
     * @brief Get the configuration associated to the central system
     * @return Configuration associated to the central system
     */
    virtual const ocpp::config::ICentralSystemConfig& getConfig() = 0;

    /**
     * @brief Get the timer pool associated to the central system
     * @return Timer pool associated to the central system
     */
    virtual ocpp::helpers::ITimerPool& getTimerPool() = 0;

    /**
     * @brief Get the worker pool associated to the central system
     * @return Worker pool associated to the central system
     */
    virtual ocpp::helpers::WorkerThreadPool& getWorkerPool() = 0;

    /**
     * @brief Reset the central system's internal data (can be done only when the central system is stopped)
     * @return true if the data has been reset, false otherwise
     */
    virtual bool resetData() = 0;

    /**
     * @brief Start the central system
     * @return true if the central system has been started, false otherwise
     */
    virtual bool start() = 0;

    /**
     * @brief Stop the central system
     * @return true if the central system has been stopped, false otherwise
     */
    virtual bool stop() = 0;

    /** @brief Interface for charge point proxy implementations */
    class IChargePoint
    {
      public:
        /** @brief Destructor */
        virtual ~IChargePoint() { }

        /**
         * @brief Get the central system instance associated to the charge point
         * @return Central system instance associated to the charge point
         */
        virtual ICentralSystem& centralSystem() = 0;

        /**
         * @brief Get the IP address of the charge point
         * @return IP address of the charge point
         */
        virtual const std::string& ipAddress() const = 0;

        /**
         * @brief Get the charge point identifier
         * @return charge point identifier
         */
        virtual const std::string& identifier() const = 0;

        /**
         * @brief Set the call request timeout
         * @param timeout New timeout value
         */
        virtual void setTimeout(std::chrono::milliseconds timeout) = 0;

        /** @brief Disconnect the charge point */
        virtual void disconnect() = 0;

        /** @brief Register the event handler */
        virtual void registerHandler(IChargePointRequestHandler& handler) = 0;

        // OCPP operations

        /**
         * @brief Cancel a reservation
         * @param reservation_id Id of the reservation
         * @return true if the reservation has been canceled, false otherwise
         */
        virtual bool cancelReservation(int reservation_id) = 0;

        /**
         * @brief Change the availability state of a connector
         * @param connector_id Id of the connector
         * @param availability Availability state (see AvailabilityType documentation)
         * @return Operation status (see AvailabilityStatus documentation)
         */
        virtual ocpp::types::ocpp16::AvailabilityStatus changeAvailability(unsigned int                          connector_id,
                                                                           ocpp::types::ocpp16::AvailabilityType availability) = 0;

        /**
         * @brief Change the value of a configuration key
         * @param key Configuration key to change
         * @param value New value
         * @return Operation status (see ConfigurationStatus documentation)
         */
        virtual ocpp::types::ocpp16::ConfigurationStatus changeConfiguration(const std::string& key, const std::string& value) = 0;

        /**
         * @brief Clear the authentication cache
         * @return true if the cache has been cleared, false otherwise
         */
        virtual bool clearCache() = 0;

        /**
         * @brief Clear 1 or more charging profiles
         * @param profile_id Id of the profile
         * @param connector_id Id of the connector
         * @param purpose Purpose of the charging profile
         * @param stack_level Stack level of the charging profile
         * @return true if at least one charging profile has been cleared, false otherwise
         */
        virtual bool clearChargingProfile(const ocpp::types::Optional<int>&                                             profile_id,
                                          const ocpp::types::Optional<unsigned int>&                                    connector_id,
                                          const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfilePurposeType>& purpose,
                                          const ocpp::types::Optional<unsigned int>&                                    stack_level) = 0;

        /**
         * @brief Send a data transfer request
         * @param vendor_id Identifies the vendor specific implementation
         * @param message_id Identifies the message
         * @param request_data Data associated to the request
         * @param status Response status (see DataTransferStatus documentation)
         * @param response_data Data associated with the response
         * @return true if the data transfer has been done, false otherwise
         */
        virtual bool dataTransfer(const std::string&                       vendor_id,
                                  const std::string&                       message_id,
                                  const std::string&                       request_data,
                                  ocpp::types::ocpp16::DataTransferStatus& status,
                                  std::string&                             response_data) = 0;
        /**
         * @brief Get a smart charging composite schedule
         * @param connector_id Id of the connector for which the schedule is requested
         * @param duration Length of the requested schedule
         * @param unit Charging rate unit for the schedule
         * @param schedule_connector_id Connector on which the schedule applies
         * @param schedule_start Periods contained in the scheduleare relative to this point in time
         * @param schedule Computed composite schedule
         * @return true if the composite schedule has been computed, false otherwise
         */
        virtual bool getCompositeSchedule(unsigned int                                                            connector_id,
                                          std::chrono::seconds                                                    duration,
                                          const ocpp::types::Optional<ocpp::types::ocpp16::ChargingRateUnitType>& unit,
                                          ocpp::types::Optional<unsigned int>&                                    schedule_connector_id,
                                          ocpp::types::Optional<ocpp::types::DateTime>&                           schedule_start,
                                          ocpp::types::Optional<ocpp::types::ocpp16::ChargingSchedule>&           schedule) = 0;

        /**
         * @brief Get the value of the configuration keys
         * @param keys List of configuration keys to retrieve (empty = whole configuration)
         * @param config_keys Configuration keys with their values
         * @param unknown_keys Unknown configuration keys
         * @return true if the configuration has been retrieved, false otherwise
         */
        virtual bool getConfiguration(const std::vector<std::string>&             keys,
                                      std::vector<ocpp::types::ocpp16::KeyValue>& config_keys,
                                      std::vector<std::string>&                   unknown_keys) = 0;

        /**
         * @brief Get the diagnostic file
         * @param uri URI where the diagnostic file shall be uploaded
         * @param retries Number of retries
         * @param retry_interval Interval between 2 retries
         * @param start Date and time of the oldest logging information to include in the diagnostics
         * @param stop Date and time of the latest logging information to include in the diagnostics
         * @param diagnotic_filename Name of the diagnostic file which will be uploaded
         * @return true if the operation has started, false otherwise
         */
        virtual bool getDiagnostics(const std::string&                                  uri,
                                    const ocpp::types::Optional<unsigned int>&          retries,
                                    const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                                    const ocpp::types::Optional<ocpp::types::DateTime>& start,
                                    const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                                    std::string&                                        diagnotic_filename) = 0;

        /**
         * @brief Get the version of the local authorization list
         * @param version Version of the local authorization list
         * @return true if the version has been retrieved, false otherwise
         */
        virtual bool getLocalListVersion(int& version) = 0;

        /**
         * @brief Send a remote start transaction request
         * @param connector_id Connector on which to start the transaction
         * @param id_tag Identifier that charge point must use to start a transaction
         * @param profile Profile to be used for the requested transaction
         * @return true if the request has been accepted, false otherwise
         */
        virtual bool remoteStartTransaction(const ocpp::types::Optional<unsigned int>&                         connector_id,
                                            const std::string&                                                 id_tag,
                                            const ocpp::types::Optional<ocpp::types::ocpp16::ChargingProfile>& profile) = 0;

        /**
         * @brief Send a remote stop transaction request
         * @param transaction_id Id of the transaction to stop
         * @return true if the request has been accepted, false otherwise
         */
        virtual bool remoteStopTransaction(int transaction_id) = 0;

        /**
         * @brief Reserve a connector
         * @param connector_id Id of the connector to be reserved
         * @param expiry_date Date and time when the reservation ends
         * @param id_tag Identifier for which the Charge Point has to reserve a connector
         * @param parent_id_tag Parent id tag
         * @param reservation_id Unique id for this reservation
         * @return Operation status (see ReservationStatus documentation)
         */
        virtual ocpp::types::ocpp16::ReservationStatus reserveNow(unsigned int                 connector_id,
                                                                  const ocpp::types::DateTime& expiry_date,
                                                                  const std::string&           id_tag,
                                                                  const std::string&           parent_id_tag,
                                                                  int                          reservation_id) = 0;

        /**
         * @brief Reset the charge point
         * @param type Type of reset
         * @return true if the request has been accepted, false otherwise
         */
        virtual bool reset(ocpp::types::ocpp16::ResetType type) = 0;

        /**
         * @brief Send or upgrade a local authorization list
         * @param version Version number of the list
         * @param authorization_list New local authorization list of differential update
         * @param update_type Update type
         * @return Operation status (see UpdateStatus documentation)
         */
        virtual ocpp::types::ocpp16::UpdateStatus sendLocalList(
            int                                                        version,
            const std::vector<ocpp::types::ocpp16::AuthorizationData>& authorization_list,
            ocpp::types::ocpp16::UpdateType                            update_type) = 0;

        /**
         * @brief Set a charging profile in the charge point
         * @param connector_id Id of the connector for which the charging profile applies
         * @param profile Charging profile to apply
         * @return Operation status (see ChargingProfileStatus documentation)
         */
        virtual ocpp::types::ocpp16::ChargingProfileStatus setChargingProfile(unsigned int                                connector_id,
                                                                              const ocpp::types::ocpp16::ChargingProfile& profile) = 0;

        /**
         * @brief Request the send of a specific message
         * @param message Requested message
         * @param connector_id Id of the connector on which the message applies
         * @return Operation status (see TriggerMessageStatus documentation)
         */
        virtual ocpp::types::ocpp16::TriggerMessageStatus triggerMessage(ocpp::types::ocpp16::MessageTrigger       message,
                                                                         const ocpp::types::Optional<unsigned int> connector_id) = 0;

        /**
         * @brief Unlock a connector
         * @param connector_id Id of the connector to unlock
         * @return Operation status (see UnlockStatus documentation)
         */
        virtual ocpp::types::ocpp16::UnlockStatus unlockConnector(unsigned int connector_id) = 0;

        /**
         * @brief Update the firmware of the charge point
         * @param uri URI where to download the firmware
         * @param retries Number of retries
         * @param retrieve_date Date and time after which the charge point is allowed to download the firmware
         * @param retry_interval Interval between 2 retries
         * @return true if the operation has started, false otherwise
         */
        virtual bool updateFirmware(const std::string&                                 uri,
                                    const ocpp::types::Optional<unsigned int>&         retries,
                                    const ocpp::types::DateTime&                       retrieve_date,
                                    const ocpp::types::Optional<std::chrono::seconds>& retry_interval) = 0;

        // Security extensions

        /**
         * @brief Send a generated certificate chain after a SignCertificate request from the charge point
         * @param certificate_chain Generated certificate chain
         * @return true if the certificate chain has been accepted by the charge point, false otherwise
         */
        virtual bool certificateSigned(const ocpp::x509::Certificate& certificate_chain) = 0;

        /**
         * @brief Delete an installed CA certificate
         * @param certificate Certificate information
         * @return Operation status (see DeleteCertificateStatusEnumType documentation)
         */
        virtual ocpp::types::ocpp16::DeleteCertificateStatusEnumType deleteCertificate(
            const ocpp::types::ocpp16::CertificateHashDataType& certificate) = 0;

        /**
         * @brief Request the send of a specific message
         * @param message Requested message
         * @param connector_id Id of the connector on which the message applies
         * @return Operation status (see TriggerMessageStatus documentation)
         */
        virtual ocpp::types::ocpp16::TriggerMessageStatusEnumType extendedTriggerMessage(
            ocpp::types::ocpp16::MessageTriggerEnumType message, const ocpp::types::Optional<unsigned int> connector_id) = 0;

        /**
         * @brief Get the list of installed CA certificates
         * @param type Type of CA certificate
         * @param certificates Certificates information
         * @return true is the list has been retrieved, false otherwise
         */
        virtual bool getInstalledCertificateIds(ocpp::types::ocpp16::CertificateUseEnumType                type,
                                                std::vector<ocpp::types::ocpp16::CertificateHashDataType>& certificates) = 0;
        /**
         * @brief Get the log file
         * @param type Type of log to retrieve
         * @param request_id Id of the request
         * @param uri URI where the log file shall be uploaded
         * @param retries Number of retries
         * @param retry_interval Interval between 2 retries
         * @param start Date and time of the oldest logging information to include in the diagnostics
         * @param stop Date and time of the latest logging information to include in the diagnostics
         * @param log_filename Name of the diagnostic file which will be uploaded
         * @return true if the operation has started, false otherwise
         */
        virtual bool getLog(ocpp::types::ocpp16::LogEnumType                    type,
                            int                                                 request_id,
                            const std::string&                                  uri,
                            const ocpp::types::Optional<unsigned int>&          retries,
                            const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
                            const ocpp::types::Optional<ocpp::types::DateTime>& start,
                            const ocpp::types::Optional<ocpp::types::DateTime>& stop,
                            std::string&                                        log_filename) = 0;

        /**
         * @brief Install a CA certificate
         * @param type Type of CA certificate
         * @param certificate CA certificate to install
         * @return Operation status (see CertificateStatusEnumType documentation)
         */
        virtual ocpp::types::ocpp16::CertificateStatusEnumType installCertificate(ocpp::types::ocpp16::CertificateUseEnumType type,
                                                                                  const ocpp::x509::Certificate& certificate) = 0;

        /**
         * @brief Update the firmware of the charge point
         * @param request_id Id of the request
         * @param uri URI where to download the firmware
         * @param retries Number of retries
         * @param retrieve_date Date and time at which the charge point must download the firmware
         * @param retry_interval Interval between 2 retries
         * @param install_date Date and time at which the charge point must install the firmware
         * @param signing_certificate Certificate with which the firmware was signed
         * @param signature Base64 encoded firmware signature
         * @return Operation status (see UpdateFirmwareStatusEnumType documentation)
         */
        virtual ocpp::types::ocpp16::UpdateFirmwareStatusEnumType signedUpdateFirmware(
            int                                                 request_id,
            const std::string&                                  uri,
            const ocpp::types::Optional<unsigned int>&          retries,
            const ocpp::types::DateTime&                        retrieve_date,
            const ocpp::types::Optional<std::chrono::seconds>&  retry_interval,
            const ocpp::types::Optional<ocpp::types::DateTime>& install_date,
            const ocpp::x509::Certificate&                      signing_certificate,
            const std::string&                                  signature) = 0;

        // ISO 15118 PnC extensions

        /**
         * @brief Send a generated certificate chain after an ISO15118 SignCertificate request from the charge point
         * @param certificate_chain Generated certificate chain
         * @return true if the certificate chain has been accepted by the charge point, false otherwise
         */
        virtual bool iso15118CertificateSigned(const ocpp::x509::Certificate& certificate_chain) = 0;

        /**
         * @brief Delete an installed ISO15118 CA certificate
         * @param certificate Certificate information
         * @return Operation status (see DeleteCertificateStatusEnumType documentation)
         */
        virtual ocpp::types::ocpp16::DeleteCertificateStatusEnumType iso15118DeleteCertificate(
            const ocpp::types::ocpp16::CertificateHashDataType& certificate) = 0;

        /**
         * @brief Get the list of installed ISO15118 CA certificates
         * @param types Type of CA certificates
         * @param certificates Certificates information
         * @return true is the list has been retrieved, false otherwise
         */
        virtual bool iso15118GetInstalledCertificateIds(const std::vector<ocpp::types::ocpp16::GetCertificateIdUseEnumType>& types,
                                                        std::vector<ocpp::types::ocpp16::CertificateHashDataChainType>& certificates) = 0;

        /**
         * @brief Install an ISO15118 CA certificate
         * @param type Type of CA certificate
         * @param certificate CA certificate to install
         * @return Operation status (see InstallCertificateStatusEnumType documentation)
         */
        virtual ocpp::types::ocpp16::InstallCertificateStatusEnumType iso15118InstallCertificate(
            ocpp::types::ocpp16::InstallCertificateUseEnumType type, const ocpp::x509::Certificate& certificate) = 0;

        /**
         * @brief Request the send of an ISO15118 SignCertificate request from the charge point
         * @return true if the request has been accepted by the charge point, false otherwise
         */
        virtual bool iso15118TriggerSignCertificate() = 0;
    };
};

} // namespace centralsystem
} // namespace ocpp

#endif // OPENOCPP_ICENTRALSYSTEM_H
