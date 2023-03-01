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

#ifndef OPENOCPP_INIFILE_H
#define OPENOCPP_INIFILE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace ocpp
{
namespace helpers
{

/** @brief Represent a file in INI format */
class IniFile
{
  public:
    // Forward declaration
    class Value;

    /** @brief Default constructor */
    IniFile();
    /**
     * @brief Load constructor
     * @param path Path to the file
     * @param sync Force automatic sync to filesytem on every modification
     */
    IniFile(const std::string& path, bool sync = true);
    /** @brief Destructor */
    virtual ~IniFile();

    /**
     * @brief Load a file in INI format
     * @param path Path to the file
     * @param sync Force automatic sync to filesytem on every modification
     * @return true if the file has been opened, false otherwise
     */
    bool load(const std::string& path, bool sync = true);

    /**
     * @brief Store the data in INI format to the same file which has been used to load data
     * @return true if the data has been stored, false otherwise
     */
    bool store() const;

    /**
     * @brief Store the data in INI format to file in the filesystem
     * @param path Path to the file
     * @return true if the file has been stored, false otherwise
     */
    bool store(const std::string& path) const;

    /** @brief Clear the data */
    void clear();

    /**
     * @brief Get the list of the sections
     * @return List of the sections
     */
    std::vector<std::string> sections() const;

    /**
     *  @brief Get the list of the parameters of the selected section
     *  @param section Name of the section
     *  @return List of the parameters of the selected section
     */
    std::vector<std::string> operator[](const std::string& section) const;

    /**
     * @brief Get the value of a parameter
     * @param section Name of the section
     * @param param Name of the parameter
     * @param default_value Default value for the parameter
     * @return Value of the parameter or default_value if the parameter doesn't exist
     */
    Value get(const std::string& section, const std::string& name, const Value& default_value = Value()) const;

    /**
     * @brief Set the value of a parameter
     * @param section Name of the section
     * @param param Name of the parameter
     * @param Value of the parameter
     */
    void set(const std::string& section, const std::string& name, const Value& value);

    // Value class

    /** @brief Represent a value of a parameter */
    class Value
    {
      public:
        /** @brief Default constructor */
        Value();
        /** @brief Copy constructor */
        Value(const Value& copy);
        /** @brief Move constructor */
        Value(const Value&& move);

        /** @brief Build a value from a string */
        Value(const char* value);
        /** @brief Build a value from a string */
        Value(const std::string& value);
        /** @brief Build a value from an integer */
        Value(int value);
        /** @brief Build a value from an unsigned integer */
        Value(unsigned int value);
        /** @brief Build a value from a floating point value */
        Value(double value);
        /** @brief Build a value from a boolean */
        Value(bool value);

        /** @brief Implicit conversion operator */
        operator const std::string&() const { return m_value; }
        /** @brief Implicit compare operator */
        bool operator==(const std::string& value) const;
        /**  @brief Implicit compare operator */
        bool operator!=(const std::string& value) const;
        /**  @brief Copy operator */
        Value& operator=(const Value& copy);

        /** @brief Indicate if the value is empty */
        bool isEmpty() const;
        /** @brief Indicate if the value is an integer */
        bool isInt() const;
        /** @brief Indicate if the value is an unsigned integer */
        bool isUInt() const;
        /** @brief Indicate if the value is a floating point value */
        bool isFloat() const;
        /** @brief Indicate if the value is a boolean */
        bool isBool() const;

        /** @brief Get the value as a string */
        std::string toString() const;
        /** @brief Get the value as an integer */
        int toInt() const;
        /** @brief Get the value as an unsigned integer */
        unsigned int toUInt() const;
        /** @brief Get the value as a floating point value */
        double toFloat() const;
        /** @brief Get the value as a boolean */
        bool toBool() const;

      private:
        /** @brief Value stored as string */
        std::string m_value;
    };

  private:
    /** @brief Underlying file in the filesystem */
    std::string m_file;
    /** @brief Force automatic sync to filesytem on every modification */
    bool m_sync;

    /** @brief Data */
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_data;
};

} // namespace helpers
} // namespace ocpp

#endif // OPENOCPP_INIFILE_H
