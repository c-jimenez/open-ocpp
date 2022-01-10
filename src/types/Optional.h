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

#ifndef OPTIONAL_H
#define OPTIONAL_H

namespace ocpp
{
namespace types
{

/** @brief Represents an optionnal parameter */
template <typename T>
class Optional
{
  public:
    /** @brief Default constructor */
    Optional() : m_value(), m_is_set(false) { }

    /** @brief Copy constructor */
    Optional(const Optional& copy) : m_value(copy.m_value), m_is_set(copy.m_is_set) { }

    /** @brief Assignment constructor */
    Optional(const T& value) : m_value(value), m_is_set(true) { }

    /** @brief Move constructor */
    Optional(T&& value) : m_value(value), m_is_set(true) { }

    /**
     * @brief Copy operator
     * @param copy Optional value to copy
     * @return This instance
     */
    Optional& operator=(const Optional& copy)
    {
        m_value  = copy.m_value;
        m_is_set = copy.m_is_set;
        return (*this);
    }

    /**
     * @brief Copy operator
     * @param value Value to copy
     * @return This instance
     */
    Optional& operator=(const T& value)
    {
        m_value  = value;
        m_is_set = true;
        return (*this);
    }

    /**
     * @brief Implicit conversion operator
     * @return Underlying parameter
     */
    operator const T&() const { return m_value; }

    /**
     * @brief Implicit conversion operator
     * @return Underlying parameter
     */
    operator T&()
    {
        m_is_set = true;
        return m_value;
    }

    /**
     * @brief Get the underlying value
     * @return Underlying value
     */
    const T& value() const { return m_value; }

    /**
     * @brief Get the underlying value and indicate that the value has been set
     * @return Underlying value
     */
    T& value()
    {
        m_is_set = true;
        return m_value;
    }

    /**
     * @brief Indicate if the value has been set
     * @return true if the value has been set, false otherwise
     */
    bool isSet() const { return m_is_set; }

    /** @brief Clear the indication that the value has been set */
    void clear() { m_is_set = false; }

  private:
    /** @brief Underlying value */
    T m_value;
    /** @brief Indicate if the value has been set */
    bool m_is_set;
};

} // namespace types
} // namespace ocpp

#endif // OPTIONAL_H
