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

#include "AuthentLocalList.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Database.h"
#include "GenericMessagesConverter.h"
#include "GetLocalListVersion.h"
#include "InternalConfigManager.h"
#include "MessageDispatcherStub.h"
#include "OcppConfigStub.h"
#include "SendLocalList.h"
#include "doctest.h"

#include <filesystem>

using namespace ocpp::chargepoint;
using namespace ocpp::config;
using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::types;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database database;

TEST_SUITE("Authentication local list")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));
    }

    TEST_CASE("Full update")
    {
        OcppConfigStub           ocpp_config;
        InternalConfigManager    internal_config(database);
        GenericMessagesConverter msg_converter;
        MessageDispatcherStub    msg_dispatcher;

        ocpp_config.setConfigValue("LocalAuthListEnabled", "true");
        ocpp_config.setConfigValue("LocalAuthListMaxLength", "5");
        ocpp_config.setConfigValue("SendLocalListMaxLength", "3");
        internal_config.initDatabaseTable();

        AuthentLocalList local_list(ocpp_config, database, internal_config, msg_converter, msg_dispatcher);

        // Check register to message handler
        CHECK(msg_dispatcher.hasHandler(GET_LOCAL_LIST_VERSION_ACTION));
        CHECK(msg_dispatcher.hasHandler(SEND_LOCAL_LIST_ACTION));

        // Check version
        GetLocalListVersionReq  version_req;
        GetLocalListVersionConf version_resp;
        const char*             error_code = nullptr;
        std::string             error_message;
        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 0);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Check send local list with too much data
        SendLocalListReq send_req;
        send_req.listVersion = 1;
        send_req.updateType  = UpdateType::Full;
        AuthorizationData auth_data;
        auth_data.idTag.assign("TAG1");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        auth_data.idTagInfo.value().parentIdTag.value().assign("PARENT_TAG1");
        auth_data.idTagInfo.value().expiryDate = DateTime(DateTime::now().timestamp() + 1000);
        time_t tag1_expiry                     = auth_data.idTagInfo.value().expiryDate.value().timestamp();
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG2");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Invalid;
        auth_data.idTagInfo.value().parentIdTag.clear();
        auth_data.idTagInfo.value().expiryDate.clear();
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG3");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Blocked;
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG4");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        send_req.localAuthorizationList.push_back(auth_data);
        SendLocalListConf send_resp;

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Failed);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 0);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Send local list with valid data
        send_req.localAuthorizationList.pop_back();
        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Accepted);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 1);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Check contents
        IdTagInfo tag_info;
        CHECK(local_list.check("TAG1", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Accepted);
        CHECK(tag_info.parentIdTag.isSet());
        CHECK_EQ(tag_info.parentIdTag.value().str(), "PARENT_TAG1");
        CHECK(tag_info.expiryDate.isSet());
        CHECK_EQ(tag_info.expiryDate.value().timestamp(), tag1_expiry);
        CHECK(local_list.check("TAG2", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Invalid);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());
        CHECK(local_list.check("TAG3", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Blocked);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());
        CHECK_FALSE(local_list.check("TAG4", tag_info));

        // Second full update, check that previous data has been erased
        send_req.listVersion = 2;
        send_req.localAuthorizationList.clear();
        auth_data.idTag.assign("TAG4");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        auth_data.idTagInfo.value().parentIdTag.clear();
        auth_data.idTagInfo.value().expiryDate.clear();
        send_req.localAuthorizationList.push_back(auth_data);

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Accepted);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 2);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK_FALSE(local_list.check("TAG1", tag_info));
        CHECK_FALSE(local_list.check("TAG2", tag_info));
        CHECK_FALSE(local_list.check("TAG3", tag_info));
        CHECK(local_list.check("TAG4", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Accepted);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());

        // Full update with tagInfo missing
        send_req.listVersion = 3;
        auth_data.idTag.assign("TAG5");
        auth_data.idTagInfo.clear();
        send_req.localAuthorizationList.push_back(auth_data);

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Failed);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 2);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Send a list which is too large
        ocpp_config.setConfigValue("LocalAuthListMaxLength", "1u");
        send_req.localAuthorizationList.back().idTagInfo.value().status = AuthorizationStatus::Accepted;

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Failed);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 2);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());
    }

    TEST_CASE("Differential update")
    {
        OcppConfigStub           ocpp_config;
        InternalConfigManager    internal_config(database);
        GenericMessagesConverter msg_converter;
        MessageDispatcherStub    msg_dispatcher;

        ocpp_config.setConfigValue("LocalAuthListEnabled", "true");
        ocpp_config.setConfigValue("LocalAuthListMaxLength", "5");
        ocpp_config.setConfigValue("SendLocalListMaxLength", "5");
        internal_config.initDatabaseTable();

        AuthentLocalList local_list(ocpp_config, database, internal_config, msg_converter, msg_dispatcher);

        // Check version
        GetLocalListVersionReq  version_req;
        GetLocalListVersionConf version_resp;
        const char*             error_code = nullptr;
        std::string             error_message;
        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 2);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Check send local list with valid data
        SendLocalListReq send_req;
        send_req.listVersion = 3;
        send_req.updateType  = UpdateType::Differential;
        AuthorizationData auth_data;
        auth_data.idTag.assign("TAG4");
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG5");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG6");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Invalid;
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG7");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Blocked;
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG8");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        send_req.localAuthorizationList.push_back(auth_data);
        SendLocalListConf send_resp;

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Accepted);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 3);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Check contents
        IdTagInfo tag_info;
        CHECK_FALSE(local_list.check("TAG4", tag_info));
        CHECK(local_list.check("TAG5", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Accepted);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());
        CHECK(local_list.check("TAG6", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Invalid);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());
        CHECK(local_list.check("TAG7", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Blocked);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());
        CHECK(local_list.check("TAG8", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Accepted);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());

        // Differential update
        send_req.listVersion = 4;
        send_req.localAuthorizationList.clear();
        auth_data.idTag.assign("TAG6");
        auth_data.idTagInfo.clear();
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG7");
        auth_data.idTagInfo.value().status     = AuthorizationStatus::Blocked;
        auth_data.idTagInfo.value().expiryDate = DateTime(DateTime::now().timestamp() - 1);
        send_req.localAuthorizationList.push_back(auth_data);
        auth_data.idTag.assign("TAG8");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Blocked;
        auth_data.idTagInfo.value().parentIdTag.value().assign("PARENT_TAG8");
        auth_data.idTagInfo.value().expiryDate = DateTime(DateTime::now().timestamp() + 1000);
        time_t tag8_expiry                     = auth_data.idTagInfo.value().expiryDate.value().timestamp();
        send_req.localAuthorizationList.push_back(auth_data);

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::Accepted);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, 4);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Check contents
        CHECK(local_list.check("TAG5", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Accepted);
        CHECK_FALSE(tag_info.parentIdTag.isSet());
        CHECK_FALSE(tag_info.expiryDate.isSet());
        CHECK_FALSE(local_list.check("TAG6", tag_info));
        CHECK_FALSE(local_list.check("TAG7", tag_info));
        CHECK(local_list.check("TAG8", tag_info));
        CHECK_EQ(tag_info.status, AuthorizationStatus::Blocked);
        CHECK(tag_info.parentIdTag.isSet());
        CHECK_EQ(tag_info.parentIdTag.value().str(), "PARENT_TAG8");
        CHECK(tag_info.expiryDate.isSet());
        CHECK_EQ(tag_info.expiryDate.value().timestamp(), tag8_expiry);
    }

    TEST_CASE("Disabled")
    {
        OcppConfigStub           ocpp_config;
        InternalConfigManager    internal_config(database);
        GenericMessagesConverter msg_converter;
        MessageDispatcherStub    msg_dispatcher;

        ocpp_config.setConfigValue("LocalAuthListEnabled", "false");
        ocpp_config.setConfigValue("LocalAuthListMaxLength", "5");
        ocpp_config.setConfigValue("SendLocalListMaxLength", "3");
        internal_config.initDatabaseTable();

        AuthentLocalList local_list(ocpp_config, database, internal_config, msg_converter, msg_dispatcher);

        // Check version
        GetLocalListVersionReq  version_req;
        GetLocalListVersionConf version_resp;
        const char*             error_code = nullptr;
        std::string             error_message;
        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, -1);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        // Check send local list
        SendLocalListReq send_req;
        send_req.listVersion = 1;
        send_req.updateType  = UpdateType::Full;
        AuthorizationData auth_data;
        auth_data.idTag.assign("TAG1");
        auth_data.idTagInfo.value().status = AuthorizationStatus::Accepted;
        send_req.localAuthorizationList.push_back(auth_data);
        SendLocalListConf send_resp;

        CHECK(local_list.handleMessage(send_req, send_resp, error_code, error_message));
        CHECK_EQ(send_resp.status, UpdateStatus::NotSupported);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(local_list.handleMessage(version_req, version_resp, error_code, error_message));
        CHECK_EQ(version_resp.listVersion, -1);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
