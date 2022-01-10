/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AUTHORIZATIONDATA_H
#define AUTHORIZATIONDATA_H

#include "IdTagInfo.h"
#include "IdToken.h"
#include "Optional.h"

namespace ocpp
{
namespace types
{

/** @brief Elements that constitute an entry of a Local Authorization List update. */
struct AuthorizationData
{
    /** @brief Required. The identifier to which this authorization applies */
    IdToken idTag;
    /** @brief Optional. (Required when UpdateType is Full) This contains information about
               authorization status, expiry and parent id. For a Differential update the following
               applies: If this element is present, then this entry SHALL be added or updated in
               the Local Authorization List. If this element is absent, than the entry for this
               idtag in the Local Authorization List SHALL be deleted */
    Optional<IdTagInfo> idTagInfo;
};

} // namespace types
} // namespace ocpp

#endif // AUTHORIZATIONDATA_H
