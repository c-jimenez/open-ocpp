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

#ifndef OPENOCPP_IDATATRANSFERMANAGER_H
#define OPENOCPP_IDATATRANSFERMANAGER_H

#include "Enums.h"

#include <string>

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for DataTransfer managers implementation */
class IDataTransferManager
{
  public:
    // Forward declaration
    class IDataTransferHandler;

    /** @brief Destructor */
    virtual ~IDataTransferManager() { }

    /**
     * @brief Register a handler for a specific data transfer vendor
     * @param vendor_id Vendor id of the data transfer
     * @param handler Handler to register
     */
    virtual void registerHandler(const std::string& vendor_id, IDataTransferHandler& handler) = 0;

    /** @brief Interface for data transfer handlers implementations */
    class IDataTransferHandler
    {
      public:
        /** @brief Destructor */
        virtual ~IDataTransferHandler() { }

        /**
         * @brief Called when a data transfer request has been received
         * @param vendor_id Identifies the vendor specific implementation
         * @param message_id Identifies the message
         * @param request_data Data associated to the request
         * @param response_data Data associated with the response
         * @return Response status (see DataTransferStatus enum)
         */
        virtual ocpp::types::ocpp16::DataTransferStatus onDataTransferRequest(const std::string& vendor_id,
                                                                              const std::string& message_id,
                                                                              const std::string& request_data,
                                                                              std::string&       response_data) = 0;
    };
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_IDATATRANSFERMANAGER_H
