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
    REGISTER_CONVERTER(CancelReservation20);
    REGISTER_CONVERTER(CertificateSigned20);
    REGISTER_CONVERTER(ChangeAvailability20);
    REGISTER_CONVERTER(ClearCache20);
    REGISTER_CONVERTER(ClearChargingProfile20);
    REGISTER_CONVERTER(ClearDisplayMessage20);
    REGISTER_CONVERTER(ClearVariableMonitoring20);
    REGISTER_CONVERTER(CustomerInformation20);
    REGISTER_CONVERTER(DataTransfer20);
    REGISTER_CONVERTER(DeleteCertificate20);
    REGISTER_CONVERTER(Get15118EVCertificate20);
    REGISTER_CONVERTER(GetBaseReport20);
    REGISTER_CONVERTER(GetCertificateStatus20);
    REGISTER_CONVERTER(GetChargingProfiles20);
    REGISTER_CONVERTER(GetCompositeSchedule20);
    REGISTER_CONVERTER(GetDisplayMessages20);
    REGISTER_CONVERTER(GetInstalledCertificateIds20);
    REGISTER_CONVERTER(GetLocalListVersion20);
    REGISTER_CONVERTER(GetLog20);
    REGISTER_CONVERTER(GetMonitoringReport20);
    REGISTER_CONVERTER(GetReport20);
    REGISTER_CONVERTER(GetTransactionStatus20);
    REGISTER_CONVERTER(GetVariables20);
    REGISTER_CONVERTER(InstallCertificate20);
    REGISTER_CONVERTER(PublishFirmware20);
    REGISTER_CONVERTER(RequestStartTransaction20);
    REGISTER_CONVERTER(RequestStopTransaction20);
    REGISTER_CONVERTER(ReserveNow20);
    REGISTER_CONVERTER(Reset20);
    REGISTER_CONVERTER(SendLocalList20);
    REGISTER_CONVERTER(SetChargingProfile20);
    REGISTER_CONVERTER(SetDisplayMessage20);
    REGISTER_CONVERTER(SetMonitoringBase20);
    REGISTER_CONVERTER(SetMonitoringLevel20);
    REGISTER_CONVERTER(SetNetworkProfile20);
    REGISTER_CONVERTER(SetVariableMonitoring20);
    REGISTER_CONVERTER(SetVariables20);
    REGISTER_CONVERTER(TriggerMessage20);
    REGISTER_CONVERTER(UnlockConnector20);
    REGISTER_CONVERTER(UnpublishFirmware20);
    REGISTER_CONVERTER(UpdateFirmware20);
    REGISTER_CONVERTER(BootNotification20);
    REGISTER_CONVERTER(Authorize20);
    REGISTER_CONVERTER(ClearedChargingLimit20);
    REGISTER_CONVERTER(CostUpdated20);
    REGISTER_CONVERTER(DataTransfer20);
    REGISTER_CONVERTER(FirmwareStatusNotification20);
    REGISTER_CONVERTER(Heartbeat20);
    REGISTER_CONVERTER(LogStatusNotification20);
    REGISTER_CONVERTER(MeterValues20);
    REGISTER_CONVERTER(NotifyChargingLimit20);
    REGISTER_CONVERTER(NotifyCustomerInformation20);
    REGISTER_CONVERTER(NotifyDisplayMessages20);
    REGISTER_CONVERTER(NotifyEVChargingNeeds20);
    REGISTER_CONVERTER(NotifyEVChargingSchedule20);
    REGISTER_CONVERTER(NotifyEvent20);
    REGISTER_CONVERTER(NotifyMonitoringReport20);
    REGISTER_CONVERTER(NotifyReport20);
    REGISTER_CONVERTER(PublishFirmwareStatusNotification20);
    REGISTER_CONVERTER(ReportChargingProfiles20);
    REGISTER_CONVERTER(ReservationStatusUpdate20);
    REGISTER_CONVERTER(SecurityEventNotification20);
    REGISTER_CONVERTER(SignCertificate20);
    REGISTER_CONVERTER(StatusNotification20);
    REGISTER_CONVERTER(TransactionEvent20);
}
/** @brief Destructor */
MessagesConverter20::~MessagesConverter20()
{
    // Free memory
    DELETE_CONVERTER(CancelReservation20);
    DELETE_CONVERTER(CertificateSigned20);
    DELETE_CONVERTER(ChangeAvailability20);
    DELETE_CONVERTER(ClearCache20);
    DELETE_CONVERTER(ClearChargingProfile20);
    DELETE_CONVERTER(ClearDisplayMessage20);
    DELETE_CONVERTER(ClearVariableMonitoring20);
    DELETE_CONVERTER(CustomerInformation20);
    DELETE_CONVERTER(DataTransfer20);
    DELETE_CONVERTER(DeleteCertificate20);
    DELETE_CONVERTER(Get15118EVCertificate20);
    DELETE_CONVERTER(GetBaseReport20);
    DELETE_CONVERTER(GetCertificateStatus20);
    DELETE_CONVERTER(GetChargingProfiles20);
    DELETE_CONVERTER(GetCompositeSchedule20);
    DELETE_CONVERTER(GetDisplayMessages20);
    DELETE_CONVERTER(GetInstalledCertificateIds20);
    DELETE_CONVERTER(GetLocalListVersion20);
    DELETE_CONVERTER(GetLog20);
    DELETE_CONVERTER(GetMonitoringReport20);
    DELETE_CONVERTER(GetReport20);
    DELETE_CONVERTER(GetTransactionStatus20);
    DELETE_CONVERTER(GetVariables20);
    DELETE_CONVERTER(InstallCertificate20);
    DELETE_CONVERTER(PublishFirmware20);
    DELETE_CONVERTER(RequestStartTransaction20);
    DELETE_CONVERTER(RequestStopTransaction20);
    DELETE_CONVERTER(ReserveNow20);
    DELETE_CONVERTER(Reset20);
    DELETE_CONVERTER(SendLocalList20);
    DELETE_CONVERTER(SetChargingProfile20);
    DELETE_CONVERTER(SetDisplayMessage20);
    DELETE_CONVERTER(SetMonitoringBase20);
    DELETE_CONVERTER(SetMonitoringLevel20);
    DELETE_CONVERTER(SetNetworkProfile20);
    DELETE_CONVERTER(SetVariableMonitoring20);
    DELETE_CONVERTER(SetVariables20);
    DELETE_CONVERTER(TriggerMessage20);
    DELETE_CONVERTER(UnlockConnector20);
    DELETE_CONVERTER(UnpublishFirmware20);
    DELETE_CONVERTER(UpdateFirmware20);
    DELETE_CONVERTER(BootNotification20);
    DELETE_CONVERTER(Authorize20);
    DELETE_CONVERTER(ClearedChargingLimit20);
    DELETE_CONVERTER(CostUpdated20);
    DELETE_CONVERTER(DataTransfer20);
    DELETE_CONVERTER(FirmwareStatusNotification20);
    DELETE_CONVERTER(Heartbeat20);
    DELETE_CONVERTER(LogStatusNotification20);
    DELETE_CONVERTER(MeterValues20);
    DELETE_CONVERTER(NotifyChargingLimit20);
    DELETE_CONVERTER(NotifyCustomerInformation20);
    DELETE_CONVERTER(NotifyDisplayMessages20);
    DELETE_CONVERTER(NotifyEVChargingNeeds20);
    DELETE_CONVERTER(NotifyEVChargingSchedule20);
    DELETE_CONVERTER(NotifyEvent20);
    DELETE_CONVERTER(NotifyMonitoringReport20);
    DELETE_CONVERTER(NotifyReport20);
    DELETE_CONVERTER(PublishFirmwareStatusNotification20);
    DELETE_CONVERTER(ReportChargingProfiles20);
    DELETE_CONVERTER(ReservationStatusUpdate20);
    DELETE_CONVERTER(SecurityEventNotification20);
    DELETE_CONVERTER(SignCertificate20);
    DELETE_CONVERTER(StatusNotification20);
    DELETE_CONVERTER(TransactionEvent20);
}

} // namespace ocpp20
} // namespace messages
} // namespace ocpp