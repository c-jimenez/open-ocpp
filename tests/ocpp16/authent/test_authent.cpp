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

#include "AuthentLocalList.h"
#include "AuthentManager.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Authorize.h"
#include "ChargePointConfigStub.h"
#include "Database.h"
#include "GenericMessageSender.h"
#include "GenericMessagesConverter.h"
#include "InternalConfigManager.h"
#include "MessageDispatcherStub.h"
#include "MessagesConverter.h"
#include "MessagesValidator.h"
#include "OcppConfigStub.h"
#include "RpcStub.h"
#include "doctest_wrapper.h"

#include <filesystem>

using namespace ocpp::chargepoint;
using namespace ocpp::config;
using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::messages::ocpp16;
using namespace ocpp::rpc;
using namespace ocpp::types;
using namespace ocpp::types::ocpp16;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database              database;
ChargePointConfigStub cp_config;
OcppConfigStub        ocpp_config;
InternalConfigManager internal_config(database);
MessagesValidator     msgs_validator;

/** @brief Prepare a response to an Authorize request */
static void setAuthorizeResponse(RpcStub& rpc, const IdTagInfo& tag_info)
{
    AuthorizeConf resp;
    resp.idTagInfo = tag_info;

    rapidjson::Document json_resp;
    json_resp.Parse("{}");
    AuthorizeConfConverter converter;
    converter.setAllocator(&json_resp.GetAllocator());
    converter.toJson(resp, json_resp);

    rpc.setResponse(json_resp);
}

TEST_SUITE("Authentication component")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));

        CHECK(msgs_validator.load(SCHEMAS_DIR));
    }

    TEST_CASE("Setup config")
    {
        cp_config.setConfigValue("AuthentCacheMaxEntriesCount", "5");
        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "true");
        ocpp_config.setConfigValue("LocalAuthListEnabled", "true");
        ocpp_config.setConfigValue("LocalAuthListMaxLength", "5");
        ocpp_config.setConfigValue("SendLocalListMaxLength", "5");
        ocpp_config.setConfigValue("LocalAuthorizeOffline", "true");
        ocpp_config.setConfigValue("LocalPreAuthorize", "true");
        internal_config.initDatabaseTable();
    }

    TEST_CASE("Setup local list")
    {
        GenericMessagesConverter msg_converter;
        MessageDispatcherStub    msg_dispatcher;

        AuthentLocalList local_list(ocpp_config, database, internal_config, msg_converter, msg_dispatcher);

        SendLocalListReq send_req;
        send_req.listVersion = 1;
        send_req.updateType  = UpdateType::Full;
        AuthorizationData auth_data;
        auth_data.idTag.assign("TAG1");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        auth_data.idTagInfo.value().parentIdTag.value().assign("PARENT_TAG1");
        auth_data.idTagInfo.value().expiryDate = DateTime(DateTime::now().timestamp() + 1000);
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG2");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Invalid;
        auth_data.idTagInfo.value().parentIdTag.clear();
        auth_data.idTagInfo.value().expiryDate.clear();
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG3");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Blocked;
        send_req.localAuthorizationList.push_back(auth_data);
        SendLocalListConf send_resp;
        std::string       error_code;
        std::string       error_message;

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Accepted);
        CHECK(error_code.empty());
        CHECK(error_message.empty());
    }

    TEST_CASE("Online without local pre-authorize")
    {
        MessagesConverter     msgs_converter;
        MessageDispatcherStub msg_dispatcher;
        RpcStub               rpc;
        GenericMessageSender  msg_sender(rpc, msgs_converter, msgs_validator, std::chrono::milliseconds(1000));

        ocpp_config.setConfigValue("LocalPreAuthorize", "false");
        rpc.setConnected(true);

        AuthentManager authent_mgr(cp_config, ocpp_config, database, internal_config, msgs_converter, msg_dispatcher, msg_sender);

        IdTagInfo           tag_info;
        AuthorizationStatus status;
        std::string         parent_id;

        // Tag in local list with status invalid
        // Check that local list is not called
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG2");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG2", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG2");

        // Tag neither in cache nor local list
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG4");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG4", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG4");

        // Tag is now in the cache
        // Check that cache is not called
        tag_info.status = AuthorizationStatus::Invalid;
        tag_info.parentIdTag.clear();
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG4", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");
    }

    TEST_CASE("Online with local pre-authorize")
    {
        MessagesConverter     msgs_converter;
        MessageDispatcherStub msg_dispatcher;
        RpcStub               rpc;
        GenericMessageSender  msg_sender(rpc, msgs_converter, msgs_validator, std::chrono::milliseconds(1000));

        ocpp_config.setConfigValue("LocalPreAuthorize", "true");
        rpc.setConnected(true);

        AuthentManager authent_mgr(cp_config, ocpp_config, database, internal_config, msgs_converter, msg_dispatcher, msg_sender);

        IdTagInfo           tag_info;
        AuthorizationStatus status;
        std::string         parent_id;

        // Tag in local list with status invalid
        // Check that local list is called
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG2");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG2", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");

        // Tag in the cache
        // Tag is status Invalid in cache so online authent must be called
        // Then tag is status Accepted in the cache so online authent must not be called
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG4");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG4", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG4");

        tag_info.status = AuthorizationStatus::Invalid;
        tag_info.parentIdTag.clear();
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG4", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG4");

        // Temporary disable local list to add a tag in
        // the cache which is also in local list
        ocpp_config.setConfigValue("LocalAuthListEnabled", "false");

        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG2");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG2", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG2");

        ocpp_config.setConfigValue("LocalAuthListEnabled", "true");

        // Tag is invalid in local list and valid in cache
        // Check that local list preempts cache

        status = authent_mgr.authorize("TAG2", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");

        // Tag neither in cache nor local list
        // Disable cache, check that tag is not added to the cache
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG5");
        setAuthorizeResponse(rpc, tag_info);
        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "false");

        status = authent_mgr.authorize("TAG5", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG5");

        tag_info.status = AuthorizationStatus::Expired;
        tag_info.parentIdTag.value().assign("");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG5", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Expired);
        CHECK_EQ(parent_id, "");

        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "true");

        tag_info.status = AuthorizationStatus::Blocked;
        tag_info.parentIdTag.value().assign("PARENT_TAG5");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG5", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Blocked);
        CHECK_EQ(parent_id, "PARENT_TAG5");
    }

    TEST_CASE("Offline without local authorize offline")
    {
        MessagesConverter     msgs_converter;
        MessageDispatcherStub msg_dispatcher;
        RpcStub               rpc;
        GenericMessageSender  msg_sender(rpc, msgs_converter, msgs_validator, std::chrono::milliseconds(1000));

        ocpp_config.setConfigValue("LocalAuthorizeOffline", "false");
        rpc.setConnected(false);

        AuthentManager authent_mgr(cp_config, ocpp_config, database, internal_config, msgs_converter, msg_dispatcher, msg_sender);

        IdTagInfo           tag_info;
        AuthorizationStatus status;
        std::string         parent_id;

        // Tag in local list with status accepted
        // Check that local list is not called
        tag_info.status = AuthorizationStatus::Blocked;
        tag_info.parentIdTag.value().assign("PARENT_TAG2");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG1", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");

        // Tag in cache with status accepted
        // Check that cache is not called
        status = authent_mgr.authorize("TAG5", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");

        // Tag neither in cache nor local list
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG6");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG6", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");
    }

    TEST_CASE("Offline with local authorize offline")
    {
        MessagesConverter     msgs_converter;
        MessageDispatcherStub msg_dispatcher;
        RpcStub               rpc;
        GenericMessageSender  msg_sender(rpc, msgs_converter, msgs_validator, std::chrono::milliseconds(1000));

        ocpp_config.setConfigValue("LocalAuthorizeOffline", "true");
        rpc.setConnected(false);

        AuthentManager authent_mgr(cp_config, ocpp_config, database, internal_config, msgs_converter, msg_dispatcher, msg_sender);

        IdTagInfo           tag_info;
        AuthorizationStatus status;
        std::string         parent_id;

        // Tag in local list with status accepted
        // Check that local list is called
        tag_info.status = AuthorizationStatus::Blocked;
        tag_info.parentIdTag.value().assign("PARENT_TAG2");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG1", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Accepted);
        CHECK_EQ(parent_id, "PARENT_TAG1");

        // Disable local list
        // Check that local list is not called
        ocpp_config.setConfigValue("LocalAuthListEnabled", "false");

        status = authent_mgr.authorize("TAG1", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");

        ocpp_config.setConfigValue("LocalAuthListEnabled", "true");

        // Tag not in local list but in cache with status blocked
        // Check that cache is called
        status = authent_mgr.authorize("TAG5", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Blocked);
        CHECK_EQ(parent_id, "PARENT_TAG5");

        // Disable cache
        // Check that cache is not called
        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "false");

        status = authent_mgr.authorize("TAG5", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");

        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "true");

        // Tag neither in cache nor local list
        tag_info.status = AuthorizationStatus::Accepted;
        tag_info.parentIdTag.value().assign("PARENT_TAG6");
        setAuthorizeResponse(rpc, tag_info);

        status = authent_mgr.authorize("TAG6", parent_id);
        CHECK_EQ(status, AuthorizationStatus::Invalid);
        CHECK_EQ(parent_id, "");
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
