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

#ifndef OPENOCPP_ISECURITYMANAGER_H
#define OPENOCPP_ISECURITYMANAGER_H

#include "DateTime.h"
#include "Optional.h"

#include <string>

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for the security manager of the charge point */
class ISecurityManager
{
  public:
    /** @brief Destructor */
    virtual ~ISecurityManager() { }

    /**
     * @brief Log a security event
     * @param type Type of the security event
     * @param message Additional information about the occurred security event
     * @param critical If non-standard security event, indicates its criticity
     *                 (only critival events are forward to central system)
     * @return true if the security evenst has been logged, false otherwise
     */
    virtual bool logSecurityEvent(const std::string& type, const std::string& message, bool critical = false) = 0;

    /**
     * @brief Clear all the security events
     * @return true if the security evenst have been cleared, false otherwise
     */
    virtual bool clearSecurityEvents() = 0;

    /**
     * @brief Export security events into a file
     * @param filepath Path of the log file to generated
     * @param start_time If set, contains the date and time of the oldest logging information to
     *                   include in the log file
     * @param stop_time If set, contains the date and time of the latest logging information to
     *                  include in the log file
     * @return true if the export has been done, false otherwise
     */
    virtual bool exportSecurityEvents(const std::string&                                  filepath,
                                      const ocpp::types::Optional<ocpp::types::DateTime>& start_time,
                                      const ocpp::types::Optional<ocpp::types::DateTime>& stop_time) = 0;

    /**
     * @brief Get the installed CA certificates as PEM encoded data
     * @param type Type of certificate
     * @return Installed CA certificates as PEM encoded data
     */
    virtual std::string getCaCertificates(ocpp::types::ocpp16::CertificateUseEnumType type) = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_ISECURITYMANAGER_H
