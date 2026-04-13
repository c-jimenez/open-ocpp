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

#include "MessagesConverter.h"

#include "Authorize.h"
#include "BootNotification.h"
#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ChangeConfiguration.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "DiagnosticsStatusNotification.h"
#include "ExtendedTriggerMessage.h"
#include "FirmwareStatusNotification.h"
#include "Get15118EVCertificate.h"
#include "GetCertificateStatus.h"
#include "GetCompositeSchedule.h"
#include "GetConfiguration.h"
#include "GetDiagnostics.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "Heartbeat.h"
#include "InstallCertificate.h"
#include "Iso15118Authorize.h"
#include "Iso15118GetInstalledCertificateIds.h"
#include "Iso15118InstallCertificate.h"
#include "Iso15118TriggerMessage.h"
#include "LogStatusNotification.h"
#include "MeterValues.h"
#include "RemoteStartTransaction.h"
#include "RemoteStopTransaction.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SecurityEventNotification.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "SignCertificate.h"
#include "SignedFirmwareStatusNotification.h"
#include "SignedUpdateFirmware.h"
#include "StartTransaction.h"
#include "StatusNotification.h"
#include "StopTransaction.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UpdateFirmware.h"

/** @brief Macro to register a message converter for an OCPP action */
#define REGISTER_CONVERTER(action)                                               \
    registerRequestConverter<action##Req>(#action, *new action##ReqConverter()); \
    registerResponseConverter<action##Conf>(#action, *new action##ConfConverter())

/** @brief Macro to delete a message converter for an OCPP action */
#define DELETE_CONVERTER(action)                  \
    deleteRequestConverter<action##Req>(#action); \
    deleteResponseConverter<action##Conf>(#action)

namespace ocpp
{
namespace messages
{
namespace ocpp16
{

/** @brief Constructor */
MessagesConverter::MessagesConverter()
{
    // Register converters
    REGISTER_CONVERTER(Authorize);
    REGISTER_CONVERTER(BootNotification);
    REGISTER_CONVERTER(CancelReservation);
    REGISTER_CONVERTER(CertificateSigned);
    REGISTER_CONVERTER(ChangeAvailability);
    REGISTER_CONVERTER(ChangeConfiguration);
    REGISTER_CONVERTER(ClearCache);
    REGISTER_CONVERTER(ClearChargingProfile);
    REGISTER_CONVERTER(DataTransfer);
    REGISTER_CONVERTER(DeleteCertificate);
    REGISTER_CONVERTER(DiagnosticsStatusNotification);
    REGISTER_CONVERTER(ExtendedTriggerMessage);
    REGISTER_CONVERTER(FirmwareStatusNotification);
    REGISTER_CONVERTER(Get15118EVCertificate);
    REGISTER_CONVERTER(GetCertificateStatus);
    REGISTER_CONVERTER(GetCompositeSchedule);
    REGISTER_CONVERTER(GetConfiguration);
    REGISTER_CONVERTER(GetDiagnostics);
    REGISTER_CONVERTER(GetInstalledCertificateIds);
    REGISTER_CONVERTER(GetLocalListVersion);
    REGISTER_CONVERTER(GetLog);
    REGISTER_CONVERTER(Heartbeat);
    REGISTER_CONVERTER(InstallCertificate);
    REGISTER_CONVERTER(Iso15118Authorize);
    REGISTER_CONVERTER(Iso15118GetInstalledCertificateIds);
    REGISTER_CONVERTER(Iso15118InstallCertificate);
    REGISTER_CONVERTER(Iso15118TriggerMessage);
    REGISTER_CONVERTER(LogStatusNotification);
    REGISTER_CONVERTER(MeterValues);
    REGISTER_CONVERTER(RemoteStartTransaction);
    REGISTER_CONVERTER(RemoteStopTransaction);
    REGISTER_CONVERTER(ReserveNow);
    REGISTER_CONVERTER(Reset);
    REGISTER_CONVERTER(SecurityEventNotification);
    REGISTER_CONVERTER(SendLocalList);
    REGISTER_CONVERTER(SetChargingProfile);
    REGISTER_CONVERTER(SignCertificate);
    REGISTER_CONVERTER(SignedFirmwareStatusNotification);
    REGISTER_CONVERTER(SignedUpdateFirmware);
    REGISTER_CONVERTER(StartTransaction);
    REGISTER_CONVERTER(StatusNotification);
    REGISTER_CONVERTER(StopTransaction);
    REGISTER_CONVERTER(TriggerMessage);
    REGISTER_CONVERTER(UnlockConnector);
    REGISTER_CONVERTER(UpdateFirmware);
}
/** @brief Destructor */
MessagesConverter::~MessagesConverter()
{
    // Free memory
    DELETE_CONVERTER(Authorize);
    DELETE_CONVERTER(BootNotification);
    DELETE_CONVERTER(CancelReservation);
    DELETE_CONVERTER(CertificateSigned);
    DELETE_CONVERTER(ChangeAvailability);
    DELETE_CONVERTER(ChangeConfiguration);
    DELETE_CONVERTER(ClearCache);
    DELETE_CONVERTER(ClearChargingProfile);
    DELETE_CONVERTER(DataTransfer);
    DELETE_CONVERTER(DeleteCertificate);
    DELETE_CONVERTER(DiagnosticsStatusNotification);
    DELETE_CONVERTER(ExtendedTriggerMessage);
    DELETE_CONVERTER(FirmwareStatusNotification);
    DELETE_CONVERTER(Get15118EVCertificate);
    DELETE_CONVERTER(GetCertificateStatus);
    DELETE_CONVERTER(GetCompositeSchedule);
    DELETE_CONVERTER(GetConfiguration);
    DELETE_CONVERTER(GetDiagnostics);
    DELETE_CONVERTER(GetInstalledCertificateIds);
    DELETE_CONVERTER(GetLocalListVersion);
    DELETE_CONVERTER(GetLog);
    DELETE_CONVERTER(Heartbeat);
    DELETE_CONVERTER(InstallCertificate);
    DELETE_CONVERTER(Iso15118Authorize);
    DELETE_CONVERTER(Iso15118GetInstalledCertificateIds);
    DELETE_CONVERTER(Iso15118InstallCertificate);
    DELETE_CONVERTER(Iso15118TriggerMessage);
    DELETE_CONVERTER(LogStatusNotification);
    DELETE_CONVERTER(MeterValues);
    DELETE_CONVERTER(RemoteStartTransaction);
    DELETE_CONVERTER(RemoteStopTransaction);
    DELETE_CONVERTER(ReserveNow);
    DELETE_CONVERTER(Reset);
    DELETE_CONVERTER(SecurityEventNotification);
    DELETE_CONVERTER(SendLocalList);
    DELETE_CONVERTER(SetChargingProfile);
    DELETE_CONVERTER(SignCertificate);
    DELETE_CONVERTER(SignedFirmwareStatusNotification);
    DELETE_CONVERTER(SignedUpdateFirmware);
    DELETE_CONVERTER(StartTransaction);
    DELETE_CONVERTER(StatusNotification);
    DELETE_CONVERTER(StopTransaction);
    DELETE_CONVERTER(TriggerMessage);
    DELETE_CONVERTER(UnlockConnector);
    DELETE_CONVERTER(UpdateFirmware);
}

} // namespace ocpp16
} // namespace messages
} // namespace ocpp
