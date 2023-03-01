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

#ifndef OPENOCPP_IINTERNALCONFIGMANAGER_H
#define OPENOCPP_IINTERNALCONFIGMANAGER_H

#include <string>

namespace ocpp
{
namespace config
{

/** @brief Interface for internal configuration manager implementations */
class IInternalConfigManager
{
  public:
    /** @brief Destructor */
    virtual ~IInternalConfigManager() { }

    /**
     * @brief Check if a configuration key exists
     * @param key Configuration key
     * @return true if the key exists, false otherwise
     */
    virtual bool keyExist(const std::string& key) = 0;

    /**
     * @brief Creates a configuration key
     * @param key Configuration key
     * @param value Initial value
     * @return true if the key has been created, false otherwise
     */
    virtual bool createKey(const std::string& key, const std::string& value) = 0;

    /**
     * @brief Set the value of a configuration key
     * @param key Configuration key
     * @param value Value
     * @return true if the key has been updated, false otherwise
     */
    virtual bool setKey(const std::string& key, const std::string& value) = 0;

    /**
     * @brief Get the value of a configuration key
     * @param key Configuration key
     * @param value Value
     * @return true if the key has been found, false otherwise
     */
    virtual bool getKey(const std::string& key, std::string& value) = 0;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_IINTERNALCONFIGMANAGER_H
