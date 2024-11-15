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

#ifndef OPENOCPP_OCPP20_CONNECTOR20_H
#define OPENOCPP_OCPP20_CONNECTOR20_H

#include "ConnectorStatusEnumType20.h"
#include "DateTime.h"

#include <mutex>
#include <string>

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Contains the state of a connector in an EVSE of a Charge Point */
struct Connector
{
    /** @brief Constructor */
    Connector(unsigned int _id, std::mutex& _mutex)
        : id(_id),
          mutex(_mutex),
          status(ocpp::types::ocpp20::ConnectorStatusEnumType::Available),
          status_timestamp(ocpp::types::DateTime::now()),
          last_notified_status(status)
    {
    }

    /** @brief Id */
    unsigned int id;

    /** @brief Mutex to protect concurrent access */
    std::mutex& mutex;

    // Status notification data

    /** @brief Status */
    ocpp::types::ocpp20::ConnectorStatusEnumType status;
    /** @brief Timestamp of the last status notification */
    ocpp::types::DateTime status_timestamp;
    /** @brief Last status notified to the central system */
    ocpp::types::ocpp20::ConnectorStatusEnumType last_notified_status;
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_CONNECTOR20_H
