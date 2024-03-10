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

#include "StringHelpers.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace ocpp
{
namespace helpers
{

/** @brief Space */
const std::string SPACE_STRING = " ";

/**  @brief Helper function to convert a string to lowercase */
std::string tolower(const std::string& str)
{
    std::string ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(), [](unsigned char c) { return std::tolower(c); });
    return ret;
}

/** @brief Helper function to trim a string */
std::string& trim(std::string& str, const std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars, 0));
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

/** @brief Helper function to trim a string */
std::string& ltrim(std::string& str, const std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars, 0));
    return str;
}

/** @brief Helper function to trim a string */
std::string& rtrim(std::string& str, const std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

/** @brief Helper function to split a string */
std::vector<std::string> split(const std::string& str, char separator)
{
    std::vector<std::string> values;
    size_t                   pos       = 0;
    size_t                   first_pos = 0;
    do
    {
        pos = str.find_first_of(separator, first_pos);
        if (pos != std::string::npos)
        {
            std::string value = str.substr(first_pos, (pos - first_pos));
            values.push_back(value);

            first_pos = pos + 1;
        }
    } while (pos != std::string::npos);
    std::string value = str.substr(first_pos);
    if (!value.empty())
    {
        values.push_back(value);
    }
    return values;
}

/** @brief Helper function to replace a substring with another into a string */
std::string& replace(std::string& str, const std::string& what, const std::string& with, bool replace_all)
{
    std::size_t pos       = 0;
    std::size_t start_pos = 0;
    do
    {
        pos = str.find(what, start_pos);
        if (pos != std::string::npos)
        {
            str.replace(pos, what.size(), with);
            start_pos = pos + 1u;
        }
    } while ((pos != std::string::npos) && (replace_all));

    return str;
}

/** @brief Helper function to check if a string starts with the specified substring */
bool startsWith(const std::string& str, const std::string& substr)
{
    bool ret = false;
    if (str.size() >= substr.size())
    {
        ret = (str.find(substr, 0) == 0);
    }
    return ret;
}

/** @brief Helper function to check if a string ends with the specified substring */
bool endsWith(const std::string& str, const std::string& substr)
{
    bool ret = false;
    if (str.size() >= substr.size())
    {
        size_t start_index = str.size() - substr.size();
        ret                = (str.find(substr, start_index) == start_index);
    }
    return ret;
}

/** @brief Helper function to convert a buffer to an hexadecimal string representation */
std::string toHexString(const void* buffer, size_t size)
{
    std::stringstream ss;
    const uint8_t*    data = reinterpret_cast<const uint8_t*>(buffer);
    if (data)
    {
        ss << std::hex;
        for (size_t i = 0; i < size; i++)
        {
            ss << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << "";
        }
    }
    return ss.str();
}

/** @brief Helper function to convert an hexadecimal string representation into an array of bytes */
std::vector<uint8_t> fromHexString(const std::string& hex_string)
{
    std::vector<uint8_t> ret;
    if ((hex_string.size() & 1) == 0)
    {
        try
        {
            for (size_t i = 0; i < hex_string.size(); i += 2u)
            {
                std::string ss = hex_string.substr(i, 2u);
                for (const auto& c : ss)
                {
                    if (!((c >= '0') && (c <= '9')) && !((c >= 'a') && (c <= 'f')) && !((c >= 'A') && (c <= 'F')))
                    {
                        throw std::out_of_range(ss);
                    }
                }
                ret.push_back(static_cast<uint8_t>(std::stoul(ss, nullptr, 16)));
            }
        }
        catch (...)
        {
            ret.clear();
        }
    }
    return ret;
}

} // namespace helpers
} // namespace ocpp
