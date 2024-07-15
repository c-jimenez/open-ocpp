
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

#ifndef OPENOCPP_AUTHENTCACHE_H
#define OPENOCPP_AUTHENTCACHE_H

#include "ClearCache.h"
#include "Database.h"
#include "Enums.h"
#include "GenericMessageHandler.h"
#include "IdTagInfo.h"

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
class IOcppConfig;
} // namespace config
namespace messages
{
class IMessageDispatcher;
} // namespace messages

// Main namespace
namespace chargepoint
{

/** @brief Handle charge point authentication cache */
class AuthentCache
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::ClearCacheReq, ocpp::messages::ocpp16::ClearCacheConf>
{
  public:
    /** @brief Constructor */
    AuthentCache(const ocpp::config::IChargePointConfig&         stack_config,
                 ocpp::config::IOcppConfig&                      ocpp_config,
                 ocpp::database::Database&                       database,
                 const ocpp::messages::GenericMessagesConverter& messages_converter,
                 ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~AuthentCache();

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::ClearCacheReq& request,
                       ocpp::messages::ocpp16::ClearCacheConf&      response,
                       std::string&                                 error_code,
                       std::string&                                 error_message) override;

    // AuthentCache interface

    /**
     * @brief Look for a tag id in the cache
     * @param id_tag Id of the user's
     * @param tag_info Information for this id
     * @return true if the id has been found in the cache, false otherwise
     */
    bool check(const std::string& id_tag, ocpp::types::ocpp16::IdTagInfo& tag_info);

    /**
     * @brief Update a tag id entry in the cache
     * @param id_tag Tag to update
     * @param tag_info Tag information to update
     */
    void update(const std::string& id_tag, const ocpp::types::ocpp16::IdTagInfo& tag_info);

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;

    /** @brief Query to look for a tag in the cache */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to delete a tag in the cache */
    std::unique_ptr<ocpp::database::Database::Query> m_delete_query;
    /** @brief Query to insert a tag in the cache */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;
    /** @brief Query to update a tag in the cache */
    std::unique_ptr<ocpp::database::Database::Query> m_update_query;

    /** @brief Initialize the database table */
    void initDatabaseTable();
    /** @brief Clear the cache */
    void clear();
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_AUTHENTCACHE_H
