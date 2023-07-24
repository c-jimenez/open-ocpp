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

#ifndef OPENOCPP_DATETIME_H
#define OPENOCPP_DATETIME_H

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace ocpp
{
namespace types
{

/** @brief Date and time representation (ISO-8601 for string representation) */
class DateTime
{
  public:
    /** @brief Instanciate a date and time object with the current date and time
     *  @return Instanciated date and time
     */
    static DateTime now() { return DateTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); }

    /** @brief Default constructor */
    DateTime() : m_datetime(0) { }

    /** @brief Constructor from std::time_t */
    DateTime(const std::time_t& init) : m_datetime(init) { }

    /**
     * @brief Copy constructor
     * @param copy Object to copy
     */
    DateTime(const DateTime& copy) : m_datetime(copy.m_datetime) { }

    /**
     * @brief Assignment operator
     * @param copy Object to copy
     * @return Reference to itself
     */
    DateTime& operator=(const DateTime& copy)
    {
        m_datetime = copy.m_datetime;
        return (*this);
    }

    /**
     * @brief Assign a new value from a string representation in UTC time
     * @param value String representation
     * @return true if the new value respects the max string size, false otherwise
     */
    bool assign(const std::string& value)
    {
        bool               ret = false;
        std::istringstream ss(value);
        std::tm            t = {};
        ss >> std::get_time(&t, "%Y-%m-%dT%TZ");
        if (ss.fail())
        {
            ss.clear();
            ss.str(value);
            ss >> std::get_time(&t, "%Y-%m-%dT%T");
        }
        if (!ss.fail())
        {
#ifdef _MSC_VER
            m_datetime = _mkgmtime(&t);
#else  // _MSC_VER
            m_datetime = std::mktime(&t);
            m_datetime += t.tm_gmtoff;
            m_datetime -= (t.tm_isdst * 3600);
#endif // _MSC_VER
            ret = true;
        }
        return ret;
    }

    /**
     * @brief Implicit conversion operator
     * @return Underlying string
     */
    operator const std::time_t&() const { return m_datetime; }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true if the 2 date and time are identicals, false otherwise
     */
    bool operator==(const std::time_t& value) const { return (value == m_datetime); }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return false if the 2 date and time are identicals, true otherwise
     */
    bool operator!=(const std::time_t& value) const { return (value != m_datetime); }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true if the value to compare is greater, false otherwise
     */
    bool operator<(const std::time_t& value) const { return (m_datetime < value); }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true if the value to compare is lower, false otherwise
     */
    bool operator>(const std::time_t& value) const { return (m_datetime > value); }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true if the value to compare is greater, false otherwise
     */
    bool operator<=(const std::time_t& value) const { return (m_datetime <= value); }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true if the value to compare is lower, false otherwise
     */
    bool operator>=(const std::time_t& value) const { return (m_datetime >= value); }

    /**
     * @brief Compare operator
     * @param value Value to compare
     * @return true if the value to compare is greater, false otherwise
     */
    bool operator<(const DateTime& value) const { return (m_datetime < value.m_datetime); }

    /**
     * @brief Compare operator
     * @param value Value to compare
     * @return true if the value to compare is lower, false otherwise
     */
    bool operator>(const DateTime& value) const { return (m_datetime > value.m_datetime); }

    /**
     * @brief Compare operator
     * @param value Value to compare
     * @return true if the value to compare is greater, false otherwise
     */
    bool operator<=(const DateTime& value) const { return (m_datetime <= value.m_datetime); }

    /**
     * @brief Compare operator
     * @param value Value to compare
     * @return true if the value to compare is lower, false otherwise
     */
    bool operator>=(const DateTime& value) const { return (m_datetime >= value.m_datetime); }

    /**
     * @brief Get the string representation (ISO-8601) of the date and time
     * @return String representation (ISO-8601) of the date and time
     */
    std::string str() const
    {
        std::ostringstream ss;
        std::tm            t = {};
#ifdef _MSC_VER
        gmtime_s(&t, &m_datetime);
#else  // _MSC_VER
        gmtime_r(&m_datetime, &t);
#endif // _MSC_VER
        ss << std::put_time(&t, "%Y-%m-%dT%TZ");
        return ss.str();
    }

    /**
     * @brief Get the UNIX timestamp corresponding to the date and time
     * @return UNIX timestamp corresponding to the date and time
     */
    std::time_t timestamp() const { return m_datetime; }

    /**
     * @brief Indicate if a date and time is empty = EPOCH
     * @return true if the date and time is empty, false otherwise
     */
    bool empty() const { return (m_datetime == 0); }

  private:
    /** @brief Underlying date and time in local time */
    std::time_t m_datetime;
};

} // namespace types
} // namespace ocpp

#endif // OPENOCPP_DATETIME_H
