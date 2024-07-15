/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "OcppConfig.h"
#include "ChargePointConfig.h"

#include <map>
#include <string>

using namespace std;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;
using namespace ocpp::helpers;

/** @brief Read attribute */
#define PARAM_READ 1
/** @brief Write attribute */
#define PARAM_WRITE 2
/** @brief Read-write attribute */
#define PARAM_READ_WRITE 3
/** @brief Reboot required attribute */
#define PARAM_REBOOT 4
/** @brief OCPP standard parameter */
#define PARAM_OCPP 8

/** @brief List of configuration values with their attributes */
static const map<string, int> CONFIGURATION_VALUES = {

    //
    // Standart OCPP configuration
    //
    {"AllowOfflineTxForUnknownId", PARAM_READ_WRITE | PARAM_OCPP},
    {"AuthorizationCacheEnabled", PARAM_READ_WRITE | PARAM_OCPP},
    {"AuthorizeRemoteTxRequests", PARAM_READ_WRITE | PARAM_OCPP},
    {"BlinkRepeat", PARAM_READ_WRITE | PARAM_OCPP},
    {"ClockAlignedDataInterval", PARAM_READ_WRITE | PARAM_OCPP},
    {"ConnectionTimeOut", PARAM_READ_WRITE | PARAM_OCPP},
    {"ConnectorPhaseRotation", PARAM_READ_WRITE | PARAM_OCPP},
    {"ConnectorPhaseRotationMaxLength", PARAM_READ | PARAM_OCPP},
    {"GetConfigurationMaxKeys", PARAM_READ | PARAM_OCPP},
    {"HeartbeatInterval", PARAM_READ_WRITE | PARAM_OCPP},
    {"LightIntensity", PARAM_READ_WRITE | PARAM_OCPP},
    {"LocalAuthorizeOffline", PARAM_READ_WRITE | PARAM_OCPP},
    {"LocalPreAuthorize", PARAM_READ_WRITE | PARAM_OCPP},
    {"MaxEnergyOnInvalidId", PARAM_READ_WRITE | PARAM_OCPP},
    {"MeterValuesAlignedData", PARAM_READ_WRITE | PARAM_OCPP},
    {"MeterValuesAlignedDataMaxLength", PARAM_READ | PARAM_OCPP},
    {"MeterValuesSampledData", PARAM_READ_WRITE | PARAM_OCPP},
    {"MeterValuesSampledDataMaxLength", PARAM_READ | PARAM_OCPP},
    {"MeterValueSampleInterval", PARAM_READ_WRITE | PARAM_OCPP},
    {"MinimumStatusDuration", PARAM_READ_WRITE | PARAM_OCPP},
    {"NumberOfConnectors", PARAM_READ | PARAM_OCPP},
    {"ResetRetries", PARAM_READ_WRITE | PARAM_OCPP},
    {"StopTransactionOnEVSideDisconnect", PARAM_READ_WRITE | PARAM_OCPP},
    {"StopTransactionOnInvalidId", PARAM_READ_WRITE | PARAM_OCPP},
    {"StopTxnAlignedData", PARAM_READ_WRITE | PARAM_OCPP},
    {"StopTxnAlignedDataMaxLength", PARAM_READ | PARAM_OCPP},
    {"StopTxnSampledData", PARAM_READ_WRITE | PARAM_OCPP},
    {"StopTxnSampledDataMaxLength", PARAM_READ | PARAM_OCPP},
    {"SupportedFeatureProfiles", PARAM_READ | PARAM_OCPP},
    {"SupportedFeatureProfilesMaxLength", PARAM_READ | PARAM_OCPP},
    {"TransactionMessageAttempts", PARAM_READ_WRITE | PARAM_OCPP},
    {"TransactionMessageRetryInterval", PARAM_READ_WRITE | PARAM_OCPP},
    {"UnlockConnectorOnEVSideDisconnect", PARAM_READ_WRITE | PARAM_OCPP},
    {"WebSocketPingInterval", PARAM_READ_WRITE | PARAM_REBOOT | PARAM_OCPP},
    {"LocalAuthListEnabled", PARAM_READ_WRITE | PARAM_OCPP},
    {"LocalAuthListMaxLength", PARAM_READ | PARAM_OCPP},
    {"SendLocalListMaxLength", PARAM_READ | PARAM_OCPP},
    {"ReserveConnectorZeroSupported", PARAM_READ | PARAM_OCPP},
    {"ChargeProfileMaxStackLevel", PARAM_READ | PARAM_OCPP},
    {"ChargingScheduleAllowedChargingRateUnit", PARAM_READ | PARAM_OCPP},
    {"ChargingScheduleMaxPeriods", PARAM_READ | PARAM_OCPP},
    {"ConnectorSwitch3to1PhaseSupported", PARAM_READ | PARAM_OCPP},
    {"MaxChargingProfilesInstalled", PARAM_READ | PARAM_OCPP},
    {"MaxChargingProfilesInstalled", PARAM_READ | PARAM_OCPP},
    //
    // Security extensions
    //
    {"AdditionalRootCertificateCheck", PARAM_READ | PARAM_OCPP},
    {"AuthorizationKey", PARAM_WRITE | PARAM_OCPP},
    {"CertificateSignedMaxChainSize", PARAM_READ | PARAM_OCPP},
    {"CertificateStoreMaxLength", PARAM_READ | PARAM_OCPP},
    {"CpoName", PARAM_READ | PARAM_WRITE | PARAM_OCPP},
    {"SecurityProfile", PARAM_READ_WRITE | PARAM_OCPP},
    {"SupportedFileTransferProtocols", PARAM_READ | PARAM_OCPP},
    //
    // ISO 15118 PnC extensions
    //
    {"CentralContractValidationAllowed", PARAM_READ_WRITE | PARAM_OCPP},
    {"CertSigningWaitMinimum", PARAM_READ_WRITE | PARAM_OCPP},
    {"CertSigningRepeatTimes", PARAM_READ_WRITE | PARAM_OCPP},
    {"ContractValidationOffline", PARAM_READ_WRITE | PARAM_OCPP},
    {"Iso15118PnCEnabled", PARAM_READ_WRITE | PARAM_OCPP},

    //
    // Charge point configuration
    //
    {"ConnexionUrl", PARAM_READ_WRITE | PARAM_REBOOT},
    {"ChargePointIdentifier", PARAM_READ_WRITE | PARAM_REBOOT},
    {"FirmwareVersion", PARAM_READ}};

/** @brief Constructor */
OcppConfig::OcppConfig(ocpp::helpers::IniFile& config) : m_config(config) { }

/** @copydoc void IOcppConfig::getConfiguration(const std::vector<ocpp::types::CiStringType<50u>>&,
 *                                              std::vector<ocpp::types::ocpp16::KeyValue>&,
 *                                              std::vector<ocpp::types::CiStringType<50u>>&);
 */
void OcppConfig::getConfiguration(const std::vector<ocpp::types::CiStringType<50u>>& keys,
                                  std::vector<ocpp::types::ocpp16::KeyValue>&        values,
                                  std::vector<ocpp::types::CiStringType<50u>>&       unknown_values)
{
    if (keys.empty())
    {
        // Return all available parameters
        for (const auto& param : CONFIGURATION_VALUES)
        {
            KeyValue key_value;
            key_value.key.assign(param.first);
            key_value.readonly = ((param.second & PARAM_WRITE) == 0);
            IniFile::Value value;
            if ((param.second & PARAM_READ) != 0)
            {
                if ((param.second & PARAM_OCPP) != 0)
                {
                    value = m_config.get(OCPP_PARAMS, param.first);
                }
                else
                {
                    value = m_config.get(STACK_PARAMS, param.first);
                }
                key_value.value.value().assign(value.toString());
            }
            values.push_back(key_value);
        }
    }
    else
    {
        // Look for each requested parameter
        for (const auto& key : keys)
        {
            const auto it = CONFIGURATION_VALUES.find(key);
            if (it != CONFIGURATION_VALUES.end())
            {
                KeyValue key_value;
                key_value.key.assign(it->first);
                key_value.readonly = ((it->second & PARAM_WRITE) == 0);
                IniFile::Value value;
                if ((it->second & PARAM_READ) != 0)
                {
                    if ((it->second & PARAM_OCPP) != 0)
                    {
                        value = m_config.get(OCPP_PARAMS, it->first);
                    }
                    else
                    {
                        value = m_config.get(STACK_PARAMS, it->first);
                    }
                    key_value.value.value().assign(value.toString());
                }
                values.push_back(key_value);
            }
            else
            {
                unknown_values.push_back(key);
            }
        }
    }
}

/** @copydoc ConfigurationStatus IOcppConfig::setConfiguration(const std::string&, const std::string&) */
ocpp::types::ocpp16::ConfigurationStatus OcppConfig::setConfiguration(const std::string& key, const std::string& value)
{
    ConfigurationStatus ret = ConfigurationStatus::NotSupported;
    const auto          it  = CONFIGURATION_VALUES.find(key);
    if (it != CONFIGURATION_VALUES.end())
    {
        if ((it->second & PARAM_WRITE) != 0)
        {
            std::size_t key_is_interval = key.find("Interval");
            if (key_is_interval != std::string::npos)
            {
                std::size_t value_is_negative = value.find("-");
                if (value_is_negative != std::string::npos)
                {
                    ret = ConfigurationStatus::Rejected;
                }
            }

            if (ret != ConfigurationStatus::Rejected)
            {
                if ((it->second & PARAM_OCPP) != 0)
                {
                    m_config.set(OCPP_PARAMS, key, value);
                }
                else
                {
                    m_config.set(STACK_PARAMS, key, value);
                }
                if ((it->second & PARAM_REBOOT) != 0)
                {
                    ret = ConfigurationStatus::RebootRequired;
                }
                else
                {
                    ret = ConfigurationStatus::Accepted;
                }
            }
        }
        else
        {
            ret = ConfigurationStatus::Rejected;
        }
    }
    return ret;
}
