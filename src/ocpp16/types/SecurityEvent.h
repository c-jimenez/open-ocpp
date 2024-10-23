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

#ifndef OPENOCPP_SECURITYEVENT_H
#define OPENOCPP_SECURITYEVENT_H

namespace ocpp
{
namespace types
{
namespace ocpp16
{

// Standard OCPP security events

/** @brief The Charge Point firmware is updated */
static constexpr const char* SECEVT_FIRMWARE_UPDATED = "FirmwareUpdated";
/** @brief The authentication credentials provided by the Charge Point were rejected by the
Central System */
static constexpr const char* SECEVT_FAILED_AUTHENT_AT_CENTRAL_SYSTEM = "FailedToAuthenticateAtCentralSystem";
/** @brief The authentication credentials provided by the Central System were rejected by
the Charge Point */
static constexpr const char* SECEVT_CENTRAL_SYSTEM_FAILED_TO_AUTHENT = "CentralSystemFailedToAuthenticate";
/** @brief The system time on the Charge Point was changed */
static constexpr const char* SECEVT_SETTING_SYSTEM_TIME = "SettingSystemTime";
/** @brief The Charge Point has booted */
static constexpr const char* SECEVT_STARTUP_OF_DEVICE = "StartupOfTheDevice";
/** @brief The Charge Point was rebooted or reset */
static constexpr const char* SECEVT_RESET_REBOOT = "ResetOrReboot";
/** @brief The security log was cleared */
static constexpr const char* SECEVT_SECURITY_LOG_CLEARED = "SecurityLogWasCleared";
/** @brief Security parameters, such as keys or the security profile used, were changed */
static constexpr const char* SECEVT_RECONFIG_SECURITY_PARAMETER = "ReconfigurationOfSecurityParameters";
/** @brief The Flash or RAM memory of the Charge Point is getting full */
static constexpr const char* SECEVT_MEMORY_EXHAUSTION = "MemoryExhaustion";
/** @brief The Charge Point has received messages that are not valid OCPP messages, if signed messages, signage invalid/incorrect */
static constexpr const char* SECEVT_INVALID_MESSAGES = "InvalidMessages";
/** @brief The Charge Point has received a replayed message (other than the Central System
           trying to resend a message because it there was for example a network problem) */
static constexpr const char* SECEVT_ATTEMPTED_REPLAY_ATTACKS = "AttemptedReplayAttacks";
/** @brief The physical tamper detection sensor was triggered */
static constexpr const char* SECEVT_TAMPER_DETECTION_ACTIVATED = "TamperDetectionActivated";
/** @brief The firmware signature is not valid */
static constexpr const char* SECEVT_INVALID_FIRMWARE_SIGNATURE = "InvalidFirmwareSignature";
/** @brief The certificate used to verify the firmware signature is not valid */
static constexpr const char* SECEVT_INVALID_FIRMWARE_SIGNING_CERT = "InvalidFirmwareSigningCertificate";
/** @brief The certificate that the Central System uses was not valid or could not be verified*/
static constexpr const char* SECEVT_INVALID_CENTRAL_SYSTEM_CERT = "InvalidCentralSystemCertificate";
/** @brief The certificate sent to the Charge Point using the SignCertificate.conf message is not a valid certificate */
static constexpr const char* SECEVT_INVALID_CHARGE_POINT_CERT = "InvalidChargePointCertificate";
/** @brief The TLS version used by the Central System is lower than 1.2 and is not allowed by the security specification */
static constexpr const char* SECEVT_INVALID_TLS_VERSION = "InvalidTLSVersion";
/** @brief The Central System did only allow connections using TLS cipher suites that are not allowed by the security specification */
static constexpr const char* SECEVT_INVALID_TLS_CIPHER_SUITE = "InvalidTLSCipherSuite";

} // namespace ocpp16
} // namespace types
} // namespace ocpp

#endif // OPENOCPP_SECURITYEVENT_H
