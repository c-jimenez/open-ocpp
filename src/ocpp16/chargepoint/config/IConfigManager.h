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

#ifndef OPENOCPP_ICONFIGMANAGER_H
#define OPENOCPP_ICONFIGMANAGER_H

#include "Enums.h"

#include <functional>
#include <string>

namespace ocpp
{
namespace chargepoint
{

/** @brief Interface for the charge point configuration manager */
class IConfigManager
{
  public:
    // Forward declarations
    class IConfigChangedListener;

    /** @brief Destructor */
    virtual ~IConfigManager() { }

    /** @brief Configuration value check function */
    typedef std::function<ocpp::types::ocpp16::ConfigurationStatus(const std::string& key, const std::string& value)>
        ConfigurationValueCheckFunc;

    /**
     * @brief Register a check function for a specific configuration value before it is modified
     * @param key Name of the configuration value
     * @param func Name of the func value
     */
    virtual void registerCheckFunction(const std::string& key, ConfigurationValueCheckFunc func) = 0;

    /**
     * @brief Register a listener to be notified when a configuration value has changed
     * @param key Name of the configuration value
     * @param listener Listener to register
     */
    virtual void registerConfigChangedListener(const std::string& key, IConfigChangedListener& listener) = 0;

    /** @brief Interface for the configuration changed listeners */
    class IConfigChangedListener
    {
      public:
        /** @brief Destructor */
        virtual ~IConfigChangedListener() { }

        /**
         * @brief Called when connection a configuration value has changed
         * @param key Name of the configuration value
         */
        virtual void configurationValueChanged(const std::string& key) = 0;
    };
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CONFIGMANAGER_H
