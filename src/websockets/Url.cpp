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

#include "Url.h"

#include <regex>

namespace ocpp
{
namespace websockets
{

/** @brief Default constructor */
Url::Url() : m_url(""), m_is_valid(false), m_protocol(""), m_username(""), m_password(""), m_address(""), m_port(0), m_path("") { }

/** @brief Constructor */
Url::Url(const std::string& url)
    : m_url(url), m_is_valid(false), m_protocol(""), m_username(""), m_password(""), m_address(""), m_port(0), m_path("")
{
    // Regex to check URL validity and extract parts
    std::cmatch match;
    std::regex  re(R"(([a-z]+):\/\/(([a-zA-Z0-9_\-.]+)(:(.+))?@)?(([a-z0-9.-]+)(:([0-9]+))?)(\/.*)?)");
    m_is_valid = std::regex_match(m_url.c_str(), match, re);
    if (m_is_valid)
    {
        // Extract parts
        m_protocol = match[1].str();
        m_username = match[3].str();
        m_password = match[5].str();
        m_address  = match[7].str();

        // Convert path
        m_path = match[10].str();
        if (m_path.empty())
        {
            m_path = "/";
        }

        // Convert port
        std::string sport = match[9].str();
        if (!sport.empty())
        {
            try
            {
                m_port = std::stoul(sport);
            }
            catch (...)
            {
                m_is_valid = false;
            }
            if ((m_port == 0) || (m_port > 65535))
            {
                m_is_valid = false;
            }
        }
    }
}

/** @brief Destructor */
Url::~Url() { }

} // namespace websockets
} // namespace ocpp
