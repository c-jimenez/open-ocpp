/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#include "StatusManagerStub.h"

using namespace ocpp::types;

namespace ocpp
{
namespace chargepoint
{

/** @brief Constructor */
StatusManagerStub::StatusManagerStub()
    : m_registration_status(RegistrationStatus::Rejected), m_connected(false), m_hb_timer_reset(false), m_connectors()
{
    reset();
}

/** @brief Destructor */
StatusManagerStub::~StatusManagerStub() { }

/** @copydoc bool IStatusManager::updateConnectorStatus(unsigned int,
 *                                                      ocpp::types::ChargePointStatus,
 *                                                      ocpp::types::ChargePointErrorCode,
 *                                                      const std::string&,
 *                                                      const std::string&,
 *                                                      const std::string&)
*/
bool StatusManagerStub::updateConnectorStatus(unsigned int                      connector_id,
                                              ocpp::types::ChargePointStatus    status,
                                              ocpp::types::ChargePointErrorCode error_code,
                                              const std::string&                info,
                                              const std::string&                vendor_id,
                                              const std::string&                vendor_error)
{
    auto connector_status      = std::make_tuple(status, error_code, info, vendor_id, vendor_error);
    m_connectors[connector_id] = connector_status;
    return true;
}

/** @brief Get the status of a connector */
ocpp::types::ChargePointStatus StatusManagerStub::StatusManagerStub::getConnectorStatus(unsigned int connector_id)
{
    ChargePointStatus status = ChargePointStatus::Unavailable;

    auto iter = m_connectors.find(connector_id);
    if (iter != m_connectors.end())
    {
        status = std::get<0>(iter->second);
    }

    return status;
}

/** @brief Get the error code of a connector */
ocpp::types::ChargePointErrorCode StatusManagerStub::getConnectorErrorCode(unsigned int connector_id)
{
    ChargePointErrorCode error = ChargePointErrorCode::OtherError;

    auto iter = m_connectors.find(connector_id);
    if (iter != m_connectors.end())
    {
        error = std::get<1>(iter->second);
    }

    return error;
}

/** @brief Get the info of a connector */
std::string StatusManagerStub::getConnectorInfo(unsigned int connector_id)
{
    std::string info;

    auto iter = m_connectors.find(connector_id);
    if (iter != m_connectors.end())
    {
        info = std::get<2>(iter->second);
    }

    return info;
}

/** @brief Get the vendor id of a connector */
std::string StatusManagerStub::getConnectorVendor(unsigned int connector_id)
{
    std::string vendor;

    auto iter = m_connectors.find(connector_id);
    if (iter != m_connectors.end())
    {
        vendor = std::get<3>(iter->second);
    }

    return vendor;
}

/** @brief Get the vendor error of a connector */
std::string StatusManagerStub::getConnectorVendorError(unsigned int connector_id)
{
    std::string error;

    auto iter = m_connectors.find(connector_id);
    if (iter != m_connectors.end())
    {
        error = std::get<4>(iter->second);
    }

    return error;
}

/** @brief Reset the stub */
void StatusManagerStub::reset()
{
    m_registration_status = RegistrationStatus::Rejected;
    m_connected           = false;
    m_hb_timer_reset      = false;
    m_connectors.clear();
}

} // namespace chargepoint
} // namespace ocpp
