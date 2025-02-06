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

#ifndef OPENOCPP_CISTRINGTYPE_H
#define OPENOCPP_CISTRINGTYPE_H

#include <string>

namespace ocpp
{
namespace types
{

/** @brief Interface for the strings with a size limit */
class ICiStringType
{
  public:
    /** @brief Destructor */
    virtual ~ICiStringType() { }

    /**
     * @brief Get the size limit of the string
     * @return Size limit in bytes of the string
     */
    virtual size_t max() const = 0;

    /**
     * @brief Assign a new value to the string
     * @param value New string value
     * @return true if the new value respects the max string size, false otherwise
     */
    virtual bool assign(const std::string& value) = 0;

    /**
     * @brief Assign a new value to the string
     * @param value New string value
     * @return true if the new value respects the max string size, false otherwise
     */
    virtual bool assign(std::string&& value) = 0;

    /**
     * @brief Implicit conversion operator
     * @return Underlying string
     */
    virtual operator const std::string&() const = 0;

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true is the 2 string are identicals, false otherwise
     */
    virtual bool operator==(const std::string& value) const = 0;

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return false is the 2 string are identicals, true otherwise
     */
    virtual bool operator!=(const std::string& value) const = 0;

    /**
     * @brief Get the underlying string
     * @return Underlying string
     */
    virtual const std::string& str() const = 0;

    /**
     * @brief Get the underlying string as a C char array
     * @return Underlying string
     */
    virtual const char* c_str() const = 0;

    /**
     * @brief Indicate if the string is empty
     * @return true if the string is empty
     */
    virtual bool empty() const = 0;

    /**
     * @brief Get the size of the string
     * @return Size of the string in bytes
     */
    virtual size_t size() const = 0;
};

/** @brief Represent a string with a size limit */
template <size_t MAX_STRING_SIZE>
class CiStringType : public ICiStringType
{
  public:
    /** @brief Default constructor */
    CiStringType() : m_string() { }

    /**
     * @brief Copy constructor
     * @param copy String to copy
     */
    CiStringType(const CiStringType& copy) : m_string(copy.m_string) { }

    /**
     * @brief Move constructor
     * @param copy String to move
     */
    CiStringType(CiStringType&& move) : m_string(std::move(move.m_string)) { }

    /** @brief Destructor */
    virtual ~CiStringType() { }

    /**
     * @brief Get the size limit of the string
     * @return Size limit in bytes of the string
     */
    size_t max() const override { return MAX_STRING_SIZE; }

    /**
     * @brief Assign a new value to the string
     * @param value New string value
     * @return true if the new value respects the max string size, false otherwise
     */
    bool assign(const std::string& value) override
    {
        bool ret = false;
        if (value.size() <= MAX_STRING_SIZE)
        {
            m_string = value;
            ret      = true;
        }
        else
        {
            m_string = value.substr(0, MAX_STRING_SIZE);
        }
        return ret;
    }

    /**
     * @brief Assign a new value to the string
     * @param value New string value
     * @return true if the new value respects the max string size, false otherwise
     */
    bool assign(std::string&& value) override
    {
        bool ret = false;
        if (value.size() <= MAX_STRING_SIZE)
        {
            m_string.assign(std::move(value));
            ret = true;
        }
        else
        {
            m_string.assign(std::move(value));
            m_string.resize(MAX_STRING_SIZE);
        }
        return ret;
    }

    /**
     * @brief Copy operator
     * @param copy String to copy
     * @return Reference to itself
     */
    CiStringType& operator=(const CiStringType& copy)
    {
        m_string = copy.m_string;
        return *this;
    }

    /**
     * @brief Copy/move operator
     * @param copy String to copy/move
     * @return Reference to itself
     */
    CiStringType& operator=(CiStringType&& copy)
    {
        m_string.assign(std::move(copy.m_string));
        return *this;
    }

    /**
     * @brief Implicit conversion operator
     * @return Underlying string
     */
    operator const std::string&() const override { return m_string; }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return true is the 2 string are identicals, false otherwise
     */
    bool operator==(const std::string& value) const override { return (value == m_string); }

    /**
     * @brief Implicit compare operator
     * @param value Value to compare
     * @return false is the 2 string are identicals, true otherwise
     */
    bool operator!=(const std::string& value) const override { return (value != m_string); }

    /**
     * @brief Get the underlying string
     * @return Underlying string
     */
    const std::string& str() const override { return m_string; }

    /**
     * @brief Get the underlying string as a C char array
     * @return Underlying string
     */
    const char* c_str() const override { return m_string.c_str(); }

    /**
     * @brief Indicate if the string is empty
     * @return true if the string is empty
     */
    bool empty() const override { return m_string.empty(); }

    /**
     * @brief Get the size of the string
     * @return Size of the string in bytes
     */
    size_t size() const override { return m_string.size(); }

  private:
    /** @brief Underlying string */
    std::string m_string;
};

} // namespace types
} // namespace ocpp

#endif // OPENOCPP_CISTRINGTYPE_H
