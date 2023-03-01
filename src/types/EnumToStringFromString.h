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

#ifndef OPENOCPP_ENUMTOSTRINGFROMSTRING_H
#define OPENOCPP_ENUMTOSTRINGFROMSTRING_H

#include "StringHelpers.h"

#include <unordered_map>
#include <vector>

namespace ocpp
{
namespace types
{

/** @brief Helper class for string to enum conversion */
template <typename EnumType>
class EnumToStringFromString
{
  public:
    /** @brief Constructor */
    EnumToStringFromString(std::initializer_list<std::pair<EnumType, const char*>> mapping)
    {
        for (auto& it : mapping)
        {
            m_enum_to_string[it.first]  = it.second;
            m_string_to_enum[it.second] = it.first;
        }
    }

    /** @brief Get the string representation of the enum value */
    std::string toString(EnumType value) const
    {
        std::string ret;
        auto        it = m_enum_to_string.find(value);
        if (it != m_enum_to_string.end())
        {
            ret = it->second;
        }
        return ret;
    }

    /** @brief Get the value represented by a string */
    EnumType fromString(const std::string& str) const
    {
        EnumType ret = m_string_to_enum.begin()->second;
        auto     it  = m_string_to_enum.find(str);
        if (it != m_string_to_enum.end())
        {
            ret = it->second;
        }
        return ret;
    }

    /** @brief Get the value represented by a string */
    bool fromString(const std::string& str, EnumType& val) const
    {
        bool ret = false;
        auto it  = m_string_to_enum.find(str);
        if (it != m_string_to_enum.end())
        {
            val = it->second;
            ret = true;
        }
        return ret;
    }

  private:
    /** @brief Map for enum to string conversion */
    std::unordered_map<EnumType, std::string> m_enum_to_string;
    /** @brief Map for string to enum conversion */
    std::unordered_map<std::string, EnumType> m_string_to_enum;
};

/** @brief Helper function to get an enum list from a CSL string */
template <typename EnumType>
std::vector<EnumType> EnumsFromCsl(const std::string& csl, const EnumToStringFromString<EnumType>& helper)
{
    std::vector<EnumType> values;
    EnumType              value;
    size_t                pos       = 0;
    size_t                first_pos = 0;
    do
    {
        pos = csl.find_first_of(',', first_pos);
        if (pos != std::string::npos)
        {
            std::string value_str = csl.substr(first_pos, (pos - first_pos));
            if (helper.fromString(ocpp::helpers::trim(value_str), value))
            {
                values.push_back(value);
            }
            first_pos = pos + 1;
        }
    } while (pos != std::string::npos);
    std::string value_str = csl.substr(first_pos);
    if (helper.fromString(ocpp::helpers::trim(value_str), value))
    {
        values.push_back(value);
    }
    return values;
}

} // namespace types
} // namespace ocpp

#endif // OPENOCPP_ENUMTOSTRINGFROMSTRING_H
