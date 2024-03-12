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

#include <iomanip>
#include <regex>
#include <sstream>

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

/** @brief Encode a part of an URL using RFC3986 percent encoding */
std::string Url::encode(const std::string& url)
{
    // RFC3986 : Un reserved chars which must not be encoded
    static const char unreserved_char[] = {'-', '_', '.', '~'};

    std::stringstream encoded_url;
    encoded_url << std::uppercase << std::hex;

    for (const auto& c : url)
    {
        // RFC3986 :  Only alphanumeric and unreserved chars may be used
        //            unencoded within a URL
        bool encode = true;
        if (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || ((c >= '0') && (c <= '9')))
        {
            encode = false;
        }
        else
        {
            for (size_t i = 0; i < sizeof(unreserved_char) / sizeof(char); i++)
            {
                if (c == unreserved_char[i])
                {
                    encode = false;
                    break;
                }
            }
        }

        if (encode)
        {
            // Percent encoding
            encoded_url << '%';
            encoded_url << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
        else
        {
            // No encoding
            encoded_url << c;
        }
    }

    return encoded_url.str();
}

} // namespace websockets
} // namespace ocpp
