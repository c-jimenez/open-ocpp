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

#ifndef OPENOCPP_IAUTHENTMANAGER_H
#define OPENOCPP_IAUTHENTMANAGER_H

#include "Enums.h"
#include "IdTagInfo.h"
#include "IdTokenInfoType.h"

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for the authentication manager */
class IAuthentManager
{
  public:
    /** @brief Destructor */
    virtual ~IAuthentManager() { }

    /**
     * @brief Ask for authorization of operation
     * @param id_tag Id of the user's
     * @param parent_id If of the user's parent tag
     * @return Authorization status (see AuthorizationStatus enum)
     */
    virtual ocpp::types::ocpp16::AuthorizationStatus authorize(const std::string& id_tag, std::string& parent_id) = 0;

    /**
     * @brief Update a tag information
     * @param id_tag Id of the tag to update
     * @param id_tag_info New tag informations
     */
    virtual void update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo& tag_info) = 0;

    /**
     * @brief Ask for authorization of an ISO15118 operation
     * @param token_id Token id of the user's
     * @return Authorization status (see AuthorizationStatus enum)
     */
    virtual ocpp::types::ocpp16::AuthorizationStatus iso15118Authorize(const std::string& token_id) = 0;

    /**
     * @brief Update an ISO15118 token information
     * @param token_id Id of the token to update
     * @param token_info New token informations
     */
    virtual void iso15118Update(const std::string& token_id, const ocpp::types::ocpp16::IdTokenInfoType& token_info) = 0;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_IAUTHENTMANAGER_H
