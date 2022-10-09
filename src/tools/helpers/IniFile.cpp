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

#include "IniFile.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace ocpp
{
namespace helpers
{

/** @brief Default constructor */
IniFile::IniFile() : m_file(), m_sync(false), m_data() { }
/** @brief Load constructor */
IniFile::IniFile(const std::string& path, bool sync) : m_file(), m_sync(false), m_data()
{
    load(path, sync);
}
/** @brief Destructor */
IniFile::~IniFile() { }

/** @brief Load a file in INI format */
bool IniFile::load(const std::string& path, bool sync)
{
    bool ret = false;

    std::fstream file(path, file.in);
    if (file.is_open())
    {
        std::string line;
        std::string section;

        m_data.clear();
        ret = true;
        while (ret && std::getline(file, line))
        {
            if (line.empty())
            {
                // Empty line
            }
            else if (line[0] == '#')
            {
                // Comment
            }
            else if (line[0] == '[')
            {
                // Section
                if (line[line.size() - 1u] == ']')
                {
                    section = line.substr(1u, line.size() - 2u);
                    auto it = m_data.find(section);
                    if (it == m_data.end())
                    {
                        m_data[section] = std::unordered_map<std::string, std::string>();
                    }
                }
                else
                {
                    ret = false;
                }
            }
            else
            {
                // Parameter
                ret      = !section.empty();
                auto pos = line.find_first_of('=');
                if (pos != line.npos)
                {
                    std::string name      = line.substr(0, pos);
                    std::string value     = line.substr(pos + 1u);
                    m_data[section][name] = value;
                    ret                   = !name.empty();
                }
                else
                {
                    ret = false;
                }
            }
        }
        if (ret)
        {
            m_sync = sync;
            if (sync)
            {
                m_file = path;
            }
            else
            {
                m_file.clear();
            }
        }
        else
        {
            m_data.clear();
            m_file.clear();
            m_sync = false;
        }
    }

    return ret;
}

/** @brief Store the data in INI format to the same file which has been used to load data */
bool IniFile::store() const
{
    bool ret = false;

    if (!m_file.empty())
    {
        ret = store(m_file);
    }

    return ret;
}

/** @brief Store the data in INI format to file in the filesystem */
bool IniFile::store(const std::string& path) const
{
    bool ret = false;

    std::fstream file(path, file.trunc | file.out);
    if (file.is_open())
    {
        for (const auto& section : m_data)
        {
            file << "[" << section.first << "]" << std::endl;
            for (const auto& param : section.second)
            {
                file << param.first << "=" << param.second << std::endl;
            }
        }

        ret = true;
    }

    return ret;
}

/** @brief Clear the data */
void IniFile::clear()
{
    m_data.clear();
}

/** @brief Get the list of the sections */
std::vector<std::string> IniFile::sections() const
{
    std::vector<std::string> sections;
    sections.reserve(m_data.size());
    for (const auto& pair : m_data)
    {
        sections.push_back(pair.first);
    }
    return sections;
}

/** @brief Get the list of the parameters of the selected section */
std::vector<std::string> IniFile::operator[](const std::string& section) const
{
    std::vector<std::string> params;
    auto                     it = m_data.find(section);
    if (it != m_data.end())
    {
        params.reserve(it->second.size());
        for (const auto& pair : it->second)
        {
            params.push_back(pair.first);
        }
    }
    return params;
}

/** @brief Get the value of a parameter */
IniFile::Value IniFile::get(const std::string& section, const std::string& name, const Value& default_value) const
{
    Value ret(default_value);

    auto it_section = m_data.find(section);
    if (it_section != m_data.end())
    {
        auto it_param = it_section->second.find(name);
        if (it_param != it_section->second.end())
        {
            ret = Value(it_param->second);
        }
    }

    return ret;
}

/** @brief Set the value of a parameter */
void IniFile::set(const std::string& section, const std::string& name, const Value& value)
{
    auto it_section = m_data.find(section);
    if (it_section != m_data.end())
    {
        auto it_param = it_section->second.find(name);
        if (it_param != it_section->second.end())
        {
            it_param->second = value;
        }
        else
        {
            it_section->second[name] = value;
        }
    }
    else
    {
        std::unordered_map<std::string, std::string> new_section;
        new_section[name] = value;
        m_data[section]   = new_section;
    }
    if (m_sync)
    {
        store(m_file);
    }
}

// Value class

/** @brief Default constructor */
IniFile::Value::Value() : m_value("") { }

/** @brief Copy constructor */
IniFile::Value::Value(const Value& copy) : m_value(copy.m_value) { }

/** @brief Move constructor */
IniFile::Value::Value(const Value&& move) : m_value(move.m_value) { }

/** @brief Build a value from a string */
IniFile::Value::Value(const char* value) : m_value(value) { }

/** @brief Build a value from a string */
IniFile::Value::Value(const std::string& value) : m_value(value) { }

/** @brief Build a value from an integer */
IniFile::Value::Value(int value) : m_value("")
{
    std::stringstream ss;
    ss << value;
    m_value = ss.str();
}

/** @brief Build a value from an unsigned integer */
IniFile::Value::Value(unsigned int value) : m_value("")
{
    std::stringstream ss;
    ss << value;
    m_value = ss.str();
}

/** @brief Build a value from a floating point value */
IniFile::Value::Value(double value) : m_value("")
{
    std::stringstream ss;
    ss << value;
    m_value = ss.str();
}

/** @brief Build a value from a boolean */
IniFile::Value::Value(bool value) : m_value(value ? "true" : "false") { }

/** @brief Implicit compare operator */
bool IniFile::Value::operator==(const std::string& value) const
{
    return (m_value == value);
}

/** @brief Implicit compare operator */
bool IniFile::Value::operator!=(const std::string& value) const
{
    return (m_value != value);
}

/** @brief Copy operator */
IniFile::Value& IniFile::Value::operator=(const Value& copy)
{
    m_value = copy.m_value;
    return (*this);
}

/** @brief Indicate if the value is empty */
bool IniFile::Value::isEmpty() const
{
    return m_value.empty();
}

/** @brief Indicate if the value is an integer */
bool IniFile::Value::isInt() const
{
    bool ret = false;
    if (!isEmpty())
    {
        size_t index = 0;
        ret          = true;
        if (m_value[index] == '-')
        {
            index++;
        }
        for (; (index < m_value.size()) && ret; index++)
        {
            ret = std::isdigit(m_value[index]);
        }
    }
    return ret;
}

/** @brief Indicate if the value is an unsigned integer */
bool IniFile::Value::isUInt() const
{
    bool ret = false;
    if (isInt())
    {
        ret = (m_value[0] != '-');
    }
    return ret;
}

/** @brief Indicate if the value is a floating point value */
bool IniFile::Value::isFloat() const
{
    bool ret = false;
    if (!isEmpty())
    {
        size_t index     = 0;
        int    dot_count = 0;
        ret              = true;
        if (m_value[index] == '-')
        {
            index++;
        }
        for (; (index < m_value.size()) && ret; index++)
        {
            ret = std::isdigit(m_value[index]);
            if (!ret && (m_value[index] == '.'))
            {
                dot_count++;
                if (dot_count <= 1)
                {
                    ret = true;
                }
            }
        }
    }
    return ret;
}

/** @brief Indicate if the value is a boolean */
bool IniFile::Value::isBool() const
{
    return ((m_value == "true") || (m_value == "false"));
}

/** @brief Get the value as a string */
std::string IniFile::Value::toString() const
{
    return m_value;
}

/** @brief Get the value as an integer */
int IniFile::Value::toInt() const
{
    return std::strtol(m_value.c_str(), nullptr, 10);
}

/** @brief Get the value as an unsigned integer */
unsigned int IniFile::Value::toUInt() const
{
    return std::strtoul(m_value.c_str(), nullptr, 10);
}

/** @brief Get the value as a floating point value */
double IniFile::Value::toFloat() const
{
    return std::strtod(m_value.c_str(), nullptr);
}

/** @brief Get the value as a boolean */
bool IniFile::Value::toBool() const
{
    return ((m_value == "true") ? true : false);
}

} // namespace helpers
} // namespace ocpp
