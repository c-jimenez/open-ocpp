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

#include "RpcStub.h"

namespace ocpp
{
namespace rpc
{
/** @brief Constructor */
RpcStub::RpcStub() : m_connected(false), m_listener(nullptr), m_spy(nullptr), m_call_will_fail(false), m_response() { }
/** @brief Destructor */
RpcStub::~RpcStub() { }

/** @copydoc bool IRpc::call(const std::string&, const rapidjson::Document&, rapidjson::Document&, std::chrono::milliseconds) */
bool RpcStub::call(const std::string&         action,
                   const rapidjson::Document& payload,
                   rapidjson::Document&       response,
                   std::chrono::milliseconds  timeout)
{
    (void)timeout;

    bool ret = false;
    if (m_connected)
    {
        rapidjson::Document* doc = new rapidjson::Document();
        doc->CopyFrom(payload, doc->GetAllocator());
        m_calls.emplace_back(action, doc);
        response.CopyFrom(m_response, response.GetAllocator());

        ret = !m_call_will_fail;
    }

    return ret;
}

/** @brief Set the next response */
void RpcStub::setResponse(const rapidjson::Document& response)
{
    m_response.CopyFrom(response, m_response.GetAllocator());
}

} // namespace rpc
} // namespace ocpp
