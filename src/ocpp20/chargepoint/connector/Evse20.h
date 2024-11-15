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

#ifndef OPENOCPP_OCPP20_EVSE20_H
#define OPENOCPP_OCPP20_EVSE20_H

#include "Connector20.h"
#include "Timer.h"

namespace ocpp
{
namespace chargepoint
{
namespace ocpp20
{

/** @brief Contains the state of an EVSE in a Charge Point */
struct Evse
{
    /** @brief Constructor */
    Evse(unsigned int _id, ocpp::helpers::ITimerPool& timer_pool, unsigned int connectors_count)
        : id(_id),
          mutex(),
          connectors(),
          status(ocpp::types::ocpp20::ConnectorStatusEnumType::Available),
          status_timestamp(ocpp::types::DateTime::now()),
          transaction_id(),
          transaction_id_offline(),
          transaction_start(),
          transaction_id_token(),
          transaction_group_id_token(),
          meter_values_timer(timer_pool)
    {
        for (unsigned int i = 1u; i <= connectors_count; i++)
        {
            connectors.push_back(new Connector(i, mutex));
        }
    }

    /** @brief Id */
    unsigned int id;

    /** @brief Mutex to protect concurrent access */
    mutable std::mutex mutex;

    /** @brief Connectors */
    std::vector<Connector*> connectors;

    // Status notification data

    /** @brief Status */
    ocpp::types::ocpp20::ConnectorStatusEnumType status;
    /** @brief Timestamp of the last status notification */
    ocpp::types::DateTime status_timestamp;

    // Transaction data

    /** @brief Current transaction id */
    std::string transaction_id;
    /** @brief Transaction id for offline transactions */
    std::string transaction_id_offline;
    /** @brief Start of transaction */
    ocpp::types::DateTime transaction_start;
    /** @brief Identifier associated with the transaction */
    std::string transaction_id_token;
    /** @brief Group identifier associated with the transaction */
    std::string transaction_group_id_token;

    // Meter values

    /** @brief Timer for sampled meter values */
    ocpp::helpers::Timer meter_values_timer;
};

} // namespace ocpp20
} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_OCPP20_EVSE20_H
