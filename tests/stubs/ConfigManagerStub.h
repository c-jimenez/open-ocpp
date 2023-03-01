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

#ifndef CONFIGMANAGERSTUB_H
#define CONFIGMANAGERSTUB_H

#include "IConfigManager.h"

namespace ocpp
{
namespace chargepoint
{

/** @brief Configuration manager stub for unit tests */
class ConfigManagerStub : public IConfigManager
{
  public:
    /** @brief Constructor */
    ConfigManagerStub() : m_specific_checks(), m_listeners() { }
    /** @brief Destructor */
    virtual ~ConfigManagerStub() { }

    /** @copydoc void IConfigManager::registerCheckFunction(const std::string&, ConfigurationValueCheckFunc) */
    void registerCheckFunction(const std::string& key, ConfigurationValueCheckFunc func) override { m_specific_checks[key] = func; }

    /** @copydoc void IConfigManager::registerConfigChangedListener(const std::string&, IConfigChangedListener&) */
    void registerConfigChangedListener(const std::string& key, IConfigChangedListener& listener) override { m_listeners[key] = &listener; }

    // API

    /** @brief Check if a checker has been registered for a configuration key */
    bool isCheckerRegistered(const std::string& key) const { return (m_specific_checks.find(key) != m_specific_checks.end()); }
    /** @brief Check if a listener has been registered for a configuration key */
    bool isListenerRegistered(const std::string& key) const { return (m_listeners.find(key) != m_listeners.end()); }

    /** @brief Reset stub data */
    void reset()
    {
        m_specific_checks.clear();
        m_listeners.clear();
    }

  private:
    /** @brief Specific check functions */
    std::map<std::string, ConfigurationValueCheckFunc> m_specific_checks;
    /** @brief Configuration listeners */
    std::map<std::string, IConfigChangedListener*> m_listeners;
};

} // namespace chargepoint
} // namespace ocpp

#endif // CONFIGMANAGERSTUB_H
