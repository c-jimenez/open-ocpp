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

#ifndef OPENOCPP_INTERNALCONFIGMANAGER_H
#define OPENOCPP_INTERNALCONFIGMANAGER_H

#include "Database.h"
#include "IInternalConfigManager.h"

namespace ocpp
{
namespace config
{

/** @brief Handle stack internal configuration */
class InternalConfigManager : public IInternalConfigManager
{
  public:
    /** @brief Constructor */
    InternalConfigManager(ocpp::database::Database& database);

    /** @brief Destructor */
    virtual ~InternalConfigManager();

    // IInternalConfigManager interface

    /** @copydoc bool IInternalConfigManager::keyExist(const std::string&) */
    bool keyExist(const std::string& key) override;

    /** @copydoc bool IInternalConfigManager::createKey(const std::string&, const std::string&) */
    bool createKey(const std::string& key, const std::string& value) override;

    /** @copydoc bool IInternalConfigManager::setKey(const std::string&, const std::string&) */
    bool setKey(const std::string& key, const std::string& value) override;

    /** @copydoc bool IInternalConfigManager::getKey(const std::string&, const std::string&) */
    bool getKey(const std::string& key, std::string& value) override;

    // InternalConfigManager interface

    /** @brief Initialize the database table */
    void initDatabaseTable();

  private:
    /** @brief Database */
    ocpp::database::Database& m_database;

    /** @brief Query to look for a key in the configuration */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to insert a key in the configuration */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;
    /** @brief Query to update a key in the configuration */
    std::unique_ptr<ocpp::database::Database::Query> m_update_query;
};

} // namespace config
} // namespace ocpp

#endif // OPENOCPP_INTERNALCONFIGMANAGER_H
