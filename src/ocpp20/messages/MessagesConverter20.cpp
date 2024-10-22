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

#include "MessagesConverter20.h"
#include "Authorize.h"
#include "BootNotification.h"
#include "CancelReservation.h"
#include "CertificateSigned.h"
#include "ChangeAvailability.h"
#include "ClearCache.h"
#include "ClearChargingProfile.h"
#include "ClearDisplayMessage.h"
#include "ClearVariableMonitoring.h"
#include "ClearedChargingLimit.h"
#include "CostUpdated.h"
#include "CustomerInformation.h"
#include "DataTransfer.h"
#include "DeleteCertificate.h"
#include "FirmwareStatusNotification.h"
#include "Get15118EVCertificate.h"
#include "GetBaseReport.h"
#include "GetCertificateStatus.h"
#include "GetChargingProfiles.h"
#include "GetCompositeSchedule.h"
#include "GetDisplayMessages.h"
#include "GetInstalledCertificateIds.h"
#include "GetLocalListVersion.h"
#include "GetLog.h"
#include "GetMonitoringReport.h"
#include "GetReport.h"
#include "GetTransactionStatus.h"
#include "GetVariables.h"
#include "Heartbeat.h"
#include "InstallCertificate.h"
#include "LogStatusNotification.h"
#include "MeterValues.h"
#include "NotifyChargingLimit.h"
#include "NotifyCustomerInformation.h"
#include "NotifyDisplayMessages.h"
#include "NotifyEVChargingNeeds.h"
#include "NotifyEVChargingSchedule.h"
#include "NotifyEvent.h"
#include "NotifyMonitoringReport.h"
#include "NotifyReport.h"
#include "PublishFirmware.h"
#include "PublishFirmwareStatusNotification.h"
#include "ReportChargingProfiles.h"
#include "RequestStartTransaction.h"
#include "RequestStopTransaction.h"
#include "ReservationStatusUpdate.h"
#include "ReserveNow.h"
#include "Reset.h"
#include "SecurityEventNotification.h"
#include "SendLocalList.h"
#include "SetChargingProfile.h"
#include "SetDisplayMessage.h"
#include "SetMonitoringBase.h"
#include "SetMonitoringLevel.h"
#include "SetNetworkProfile.h"
#include "SetVariableMonitoring.h"
#include "SetVariables.h"
#include "SignCertificate.h"
#include "StatusNotification.h"
#include "TransactionEvent.h"
#include "TriggerMessage.h"
#include "UnlockConnector.h"
#include "UnpublishFirmware.h"
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
namespace ocpp20
{

/** @brief Constructor */
MessagesConverter20::MessagesConverter20()
{
    // Register converters
    REGISTER_CONVERTER(CancelReservation);
    REGISTER_CONVERTER(CertificateSigned);
    REGISTER_CONVERTER(ChangeAvailability);
    REGISTER_CONVERTER(ClearCache);
    REGISTER_CONVERTER(ClearChargingProfile);
    REGISTER_CONVERTER(ClearDisplayMessage);
    REGISTER_CONVERTER(ClearVariableMonitoring);
    REGISTER_CONVERTER(CustomerInformation);
    REGISTER_CONVERTER(DataTransfer);
    REGISTER_CONVERTER(DeleteCertificate);
    REGISTER_CONVERTER(Get15118EVCertificate);
    REGISTER_CONVERTER(GetBaseReport);
    REGISTER_CONVERTER(GetCertificateStatus);
    REGISTER_CONVERTER(GetChargingProfiles);
    REGISTER_CONVERTER(GetCompositeSchedule);
    REGISTER_CONVERTER(GetDisplayMessages);
    REGISTER_CONVERTER(GetInstalledCertificateIds);
    REGISTER_CONVERTER(GetLocalListVersion);
    REGISTER_CONVERTER(GetLog);
    REGISTER_CONVERTER(GetMonitoringReport);
    REGISTER_CONVERTER(GetReport);
    REGISTER_CONVERTER(GetTransactionStatus);
    REGISTER_CONVERTER(GetVariables);
    REGISTER_CONVERTER(InstallCertificate);
    REGISTER_CONVERTER(PublishFirmware);
    REGISTER_CONVERTER(RequestStartTransaction);
    REGISTER_CONVERTER(RequestStopTransaction);
    REGISTER_CONVERTER(ReserveNow);
    REGISTER_CONVERTER(Reset);
    REGISTER_CONVERTER(SendLocalList);
    REGISTER_CONVERTER(SetChargingProfile);
    REGISTER_CONVERTER(SetDisplayMessage);
    REGISTER_CONVERTER(SetMonitoringBase);
    REGISTER_CONVERTER(SetMonitoringLevel);
    REGISTER_CONVERTER(SetNetworkProfile);
    REGISTER_CONVERTER(SetVariableMonitoring);
    REGISTER_CONVERTER(SetVariables);
    REGISTER_CONVERTER(TriggerMessage);
    REGISTER_CONVERTER(UnlockConnector);
    REGISTER_CONVERTER(UnpublishFirmware);
    REGISTER_CONVERTER(UpdateFirmware);
    REGISTER_CONVERTER(BootNotification);
    REGISTER_CONVERTER(Authorize);
    REGISTER_CONVERTER(ClearedChargingLimit);
    REGISTER_CONVERTER(CostUpdated);
    REGISTER_CONVERTER(DataTransfer);
    REGISTER_CONVERTER(FirmwareStatusNotification);
    REGISTER_CONVERTER(Heartbeat);
    REGISTER_CONVERTER(LogStatusNotification);
    REGISTER_CONVERTER(MeterValues);
    REGISTER_CONVERTER(NotifyChargingLimit);
    REGISTER_CONVERTER(NotifyCustomerInformation);
    REGISTER_CONVERTER(NotifyDisplayMessages);
    REGISTER_CONVERTER(NotifyEVChargingNeeds);
    REGISTER_CONVERTER(NotifyEVChargingSchedule);
    REGISTER_CONVERTER(NotifyEvent);
    REGISTER_CONVERTER(NotifyMonitoringReport);
    REGISTER_CONVERTER(NotifyReport);
    REGISTER_CONVERTER(PublishFirmwareStatusNotification);
    REGISTER_CONVERTER(ReportChargingProfiles);
    REGISTER_CONVERTER(ReservationStatusUpdate);
    REGISTER_CONVERTER(SecurityEventNotification);
    REGISTER_CONVERTER(SignCertificate);
    REGISTER_CONVERTER(StatusNotification);
    REGISTER_CONVERTER(TransactionEvent);
}
/** @brief Destructor */
MessagesConverter20::~MessagesConverter20()
{
    // Free memory
    DELETE_CONVERTER(CancelReservation);
    DELETE_CONVERTER(CertificateSigned);
    DELETE_CONVERTER(ChangeAvailability);
    DELETE_CONVERTER(ClearCache);
    DELETE_CONVERTER(ClearChargingProfile);
    DELETE_CONVERTER(ClearDisplayMessage);
    DELETE_CONVERTER(ClearVariableMonitoring);
    DELETE_CONVERTER(CustomerInformation);
    DELETE_CONVERTER(DataTransfer);
    DELETE_CONVERTER(DeleteCertificate);
    DELETE_CONVERTER(Get15118EVCertificate);
    DELETE_CONVERTER(GetBaseReport);
    DELETE_CONVERTER(GetCertificateStatus);
    DELETE_CONVERTER(GetChargingProfiles);
    DELETE_CONVERTER(GetCompositeSchedule);
    DELETE_CONVERTER(GetDisplayMessages);
    DELETE_CONVERTER(GetInstalledCertificateIds);
    DELETE_CONVERTER(GetLocalListVersion);
    DELETE_CONVERTER(GetLog);
    DELETE_CONVERTER(GetMonitoringReport);
    DELETE_CONVERTER(GetReport);
    DELETE_CONVERTER(GetTransactionStatus);
    DELETE_CONVERTER(GetVariables);
    DELETE_CONVERTER(InstallCertificate);
    DELETE_CONVERTER(PublishFirmware);
    DELETE_CONVERTER(RequestStartTransaction);
    DELETE_CONVERTER(RequestStopTransaction);
    DELETE_CONVERTER(ReserveNow);
    DELETE_CONVERTER(Reset);
    DELETE_CONVERTER(SendLocalList);
    DELETE_CONVERTER(SetChargingProfile);
    DELETE_CONVERTER(SetDisplayMessage);
    DELETE_CONVERTER(SetMonitoringBase);
    DELETE_CONVERTER(SetMonitoringLevel);
    DELETE_CONVERTER(SetNetworkProfile);
    DELETE_CONVERTER(SetVariableMonitoring);
    DELETE_CONVERTER(SetVariables);
    DELETE_CONVERTER(TriggerMessage);
    DELETE_CONVERTER(UnlockConnector);
    DELETE_CONVERTER(UnpublishFirmware);
    DELETE_CONVERTER(UpdateFirmware);
    DELETE_CONVERTER(BootNotification);
    DELETE_CONVERTER(Authorize);
    DELETE_CONVERTER(ClearedChargingLimit);
    DELETE_CONVERTER(CostUpdated);
    DELETE_CONVERTER(DataTransfer);
    DELETE_CONVERTER(FirmwareStatusNotification);
    DELETE_CONVERTER(Heartbeat);
    DELETE_CONVERTER(LogStatusNotification);
    DELETE_CONVERTER(MeterValues);
    DELETE_CONVERTER(NotifyChargingLimit);
    DELETE_CONVERTER(NotifyCustomerInformation);
    DELETE_CONVERTER(NotifyDisplayMessages);
    DELETE_CONVERTER(NotifyEVChargingNeeds);
    DELETE_CONVERTER(NotifyEVChargingSchedule);
    DELETE_CONVERTER(NotifyEvent);
    DELETE_CONVERTER(NotifyMonitoringReport);
    DELETE_CONVERTER(NotifyReport);
    DELETE_CONVERTER(PublishFirmwareStatusNotification);
    DELETE_CONVERTER(ReportChargingProfiles);
    DELETE_CONVERTER(ReservationStatusUpdate);
    DELETE_CONVERTER(SecurityEventNotification);
    DELETE_CONVERTER(SignCertificate);
    DELETE_CONVERTER(StatusNotification);
    DELETE_CONVERTER(TransactionEvent);
}

} // namespace ocpp20
} // namespace messages
} // namespace ocpp