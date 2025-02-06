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
#include "Authorize20.h"
#include "BootNotification20.h"
#include "CancelReservation20.h"
#include "CertificateSigned20.h"
#include "ChangeAvailability20.h"
#include "ClearCache20.h"
#include "ClearChargingProfile20.h"
#include "ClearDisplayMessage20.h"
#include "ClearVariableMonitoring20.h"
#include "ClearedChargingLimit20.h"
#include "CostUpdated20.h"
#include "CustomerInformation20.h"
#include "DataTransfer20.h"
#include "DeleteCertificate20.h"
#include "FirmwareStatusNotification20.h"
#include "Get15118EVCertificate20.h"
#include "GetBaseReport20.h"
#include "GetCertificateStatus20.h"
#include "GetChargingProfiles20.h"
#include "GetCompositeSchedule20.h"
#include "GetDisplayMessages20.h"
#include "GetInstalledCertificateIds20.h"
#include "GetLocalListVersion20.h"
#include "GetLog20.h"
#include "GetMonitoringReport20.h"
#include "GetReport20.h"
#include "GetTransactionStatus20.h"
#include "GetVariables20.h"
#include "Heartbeat20.h"
#include "InstallCertificate20.h"
#include "LogStatusNotification20.h"
#include "MeterValues20.h"
#include "NotifyChargingLimit20.h"
#include "NotifyCustomerInformation20.h"
#include "NotifyDisplayMessages20.h"
#include "NotifyEVChargingNeeds20.h"
#include "NotifyEVChargingSchedule20.h"
#include "NotifyEvent20.h"
#include "NotifyMonitoringReport20.h"
#include "NotifyReport20.h"
#include "PublishFirmware20.h"
#include "PublishFirmwareStatusNotification20.h"
#include "ReportChargingProfiles20.h"
#include "RequestStartTransaction20.h"
#include "RequestStopTransaction20.h"
#include "ReservationStatusUpdate20.h"
#include "ReserveNow20.h"
#include "Reset20.h"
#include "SecurityEventNotification20.h"
#include "SendLocalList20.h"
#include "SetChargingProfile20.h"
#include "SetDisplayMessage20.h"
#include "SetMonitoringBase20.h"
#include "SetMonitoringLevel20.h"
#include "SetNetworkProfile20.h"
#include "SetVariableMonitoring20.h"
#include "SetVariables20.h"
#include "SignCertificate20.h"
#include "StatusNotification20.h"
#include "TransactionEvent20.h"
#include "TriggerMessage20.h"
#include "UnlockConnector20.h"
#include "UnpublishFirmware20.h"
#include "UpdateFirmware20.h"

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