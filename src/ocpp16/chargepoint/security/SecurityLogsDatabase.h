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

#ifndef OPENOCPP_SECURITYLOGSDATABASE_H
#define OPENOCPP_SECURITYLOGSDATABASE_H

#include "Database.h"
#include "DateTime.h"
#include "Optional.h"

#include <memory>

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
} // namespace config

// Main namespace
namespace chargepoint
{

/** @brief Handle persistency of security logs */
class SecurityLogsDatabase
{
  public:
    /** @brief Constructor */
    SecurityLogsDatabase(const ocpp::config::IChargePointConfig& stack_config, ocpp::database::Database& database);

    /** @brief Destructor */
    virtual ~SecurityLogsDatabase();

    // SecurityLogsDatabase interface

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /**
     * @brief Log a security event
     * @param type Type of the security event
     * @param message Additional information about the occurred security event
     * @param critical If non-standard security event, indicates its criticity
     *                 (only critival events are forward to central system)
     * @param timestamp Timestamp of the security event
     * @return true if the security evenst has been logged, false otherwise
     */
    bool log(const std::string& type, const std::string& message, bool critical, const ocpp::types::DateTime& timestamp);

    /**
     * @brief Clear all the security events
     * @return true if the security evenst have been cleared, false otherwise
     */
    bool clear();

    /**
     * @brief Export security events into a file
     * @param filepath Path of the log file to generated
     * @param start_time If set, contains the date and time of the oldest logging information to
     *                   include in the log file
     * @param stop_time If set, contains the date and time of the latest logging information to
     *                  include in the log file
     * @return true if the export has been done, false otherwise
     */
    bool exportSecurityEvents(const std::string&                                  filepath,
                              const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                              const ocpp::types::Optional<ocpp::types::DateTime>& stop_time);

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;

    /** @brief Query to clear all the security logs */
    std::unique_ptr<ocpp::database::Database::Query> m_clear_query;
    /** @brief Query to insert a security log */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_SECURITYLOGSDATABASE_H
