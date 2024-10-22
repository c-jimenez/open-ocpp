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

#include "GenericMessageSender.h"

namespace ocpp
{
namespace messages
{

/** @brief Helper to convert a CallResult enum to string */
const ocpp::types::EnumToStringFromString<CallResult> CallResultHelper = {
    {CallResult::Ok, "Ok"}, {CallResult::Delayed, "Delayed"}, {CallResult::Failed, "Failed"}, {CallResult::Error, "Error"}};

} // namespace messages
} // namespace ocpp