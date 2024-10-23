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

#ifndef OPENOCPP_AUTHENTLOCALLIST_H
#define OPENOCPP_AUTHENTLOCALLIST_H

#include "Database.h"
#include "Enums.h"
#include "GenericMessageHandler.h"
#include "GetLocalListVersion.h"
#include "SendLocalList.h"

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
class IOcppConfig;
class IInternalConfigManager;
} // namespace config
namespace messages
{
class IMessageDispatcher;
} // namespace messages

// Main namespace
namespace chargepoint
{

/** @brief Handle charge point authentication local list */
class AuthentLocalList
    : public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::GetLocalListVersionReq,
                                                   ocpp::messages::ocpp16::GetLocalListVersionConf>,
      public ocpp::messages::GenericMessageHandler<ocpp::messages::ocpp16::SendLocalListReq, ocpp::messages::ocpp16::SendLocalListConf>
{
  public:
    /** @brief Constructor */
    AuthentLocalList(ocpp::config::IOcppConfig&                      ocpp_config,
                     ocpp::database::Database&                       database,
                     ocpp::config::IInternalConfigManager&           internal_config,
                     const ocpp::messages::GenericMessagesConverter& messages_converter,
                     ocpp::messages::IMessageDispatcher&             msg_dispatcher);

    /** @brief Destructor */
    virtual ~AuthentLocalList();

    // GenericMessageHandler interface

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::GetLocalListVersionReq& request,
                       ocpp::messages::ocpp16::GetLocalListVersionConf&      response,
                       std::string&                                          error_code,
                       std::string&                                          error_message) override;

    /** @copydoc bool GenericMessageHandler<RequestType, ResponseType>::handleMessage(const RequestType& request,
     *                                                                                ResponseType& response,
     *                                                                                std::string& error_code,
     *                                                                                std::string& error_message)
     */
    bool handleMessage(const ocpp::messages::ocpp16::SendLocalListReq& request,
                       ocpp::messages::ocpp16::SendLocalListConf&      response,
                       std::string&                                    error_code,
                       std::string&                                    error_message) override;

    // AuthentLocalList interface

    /**
     * @brief Look for a tag id in the local list
     * @param id_tag Id of the user's
     * @param tag_info Information for this id
     * @return true if the id has been found in the local list, false otherwise
     */
    bool check(const std::string& id_tag, ocpp::types::ocpp16::IdTagInfo& tag_info);

  private:
    /** @brief Standard OCPP configuration */
    ocpp::config::IOcppConfig& m_ocpp_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;
    /** @brief Charge point's internal configuration */
    ocpp::config::IInternalConfigManager& m_internal_config;

    /** @brief Current local list version */
    int m_local_list_version;

    /** @brief Query to look for a tag in the local list */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to delete a tag in the local list */
    std::unique_ptr<ocpp::database::Database::Query> m_delete_query;
    /** @brief Query to insert a tag in the local list */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;
    /** @brief Query to update a tag in the local list */
    std::unique_ptr<ocpp::database::Database::Query> m_update_query;

    /** @brief Initialize the database table */
    void initDatabaseTable();
    /** @brief Perform the full update of the local list */
    bool performFullUpdate(const std::vector<ocpp::types::ocpp16::AuthorizationData>& authorization_datas);
    /** @brief Perform the partial update of the local list */
    bool performPartialUpdate(const std::vector<ocpp::types::ocpp16::AuthorizationData>& authorization_datas);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_AUTHENTLOCALLIST_H
