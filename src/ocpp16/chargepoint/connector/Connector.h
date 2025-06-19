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

#ifndef OPENOCPP_CONNECTOR_H
#define OPENOCPP_CONNECTOR_H

#include "DateTime.h"
#include "Enums.h"
#include "Timer.h"

#include <mutex>
#include <string>

namespace ocpp
{
namespace chargepoint
{

/** @brief Contains the state of a connector in a Charge Point */
struct Connector
{
    /** @brief Constructor */
    Connector(unsigned int _id, ocpp::helpers::ITimerPool& timer_pool)
        : id(_id),
          mutex(),
          availability(ocpp::types::ocpp16::AvailabilityType::Operative),
          status(ocpp::types::ocpp16::ChargePointStatus::Available),
          error_code(ocpp::types::ocpp16::ChargePointErrorCode::NoError),
          status_timestamp(ocpp::types::DateTime::now()),
          info(),
          vendor_id(),
          vendor_error(),
          status_timer(timer_pool),
          last_notified_status(status),
          transaction_id(0),
          transaction_id_offline(0),
          transaction_start(),
          transaction_id_tag(),
          reservation_id(0),
          reservation_id_tag(),
          reservation_parent_id_tag(),
          reservation_expiry_date(),
          meter_values_timer(timer_pool)
    {
    }

    /** @brief Id */
    unsigned int id;

    /** @brief Mutex to protect concurrent access */
    mutable std::mutex mutex;

    // Status notification data

    /** @brief Availability */
    ocpp::types::ocpp16::AvailabilityType availability;
    /** @brief Status */
    ocpp::types::ocpp16::ChargePointStatus status;
    /** @brief Error code */
    ocpp::types::ocpp16::ChargePointErrorCode error_code;
    /** @brief Timestamp of the last status notification */
    ocpp::types::DateTime status_timestamp;
    /** @brief Info */
    std::string info;
    /** @brief Vendor id */
    std::string vendor_id;
    /** @brief Vendor error */
    std::string vendor_error;
    /** @brief Timer for status duration */
    ocpp::helpers::Timer status_timer;
    /** @brief Last status notified to the central system */
    ocpp::types::ocpp16::ChargePointStatus last_notified_status;

    // Transaction data

    /** @brief Current transaction id */
    int transaction_id;
    /** @brief Transaction id for offline transactions */
    int transaction_id_offline;
    /** @brief Start of transaction */
    ocpp::types::DateTime transaction_start;
    /** @brief Id tag associated with the transaction */
    std::string transaction_id_tag;
    /** @brief Parent id tag associated with the transaction */
    std::string transaction_parent_id_tag;

    // Reservation data

    /** @brief Current reservation id */
    int reservation_id;
    /** @brief Id tag associated with the reservation */
    std::string reservation_id_tag;
    /** @brief Parent id tag associated with the reservation */
    std::string reservation_parent_id_tag;
    /** @brief Reservation's expiry date */
    ocpp::types::DateTime reservation_expiry_date;

    // Meter values

    /** @brief Timer for sampled meter values */
    ocpp::helpers::Timer meter_values_timer;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CONNECTOR_H
