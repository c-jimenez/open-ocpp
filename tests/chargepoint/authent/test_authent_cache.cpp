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

#include "AuthentCache.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "ChargePointConfigStub.h"
#include "ClearCache.h"
#include "Database.h"
#include "GenericMessagesConverter.h"
#include "MessageDispatcherStub.h"
#include "OcppConfigStub.h"
#include "doctest.h"

#include <filesystem>

using namespace ocpp::chargepoint;
using namespace ocpp::config;
using namespace ocpp::database;
using namespace ocpp::messages;
using namespace ocpp::types;

static constexpr const char* DATABASE_PATH = "/tmp/test.db";

Database database;

TEST_SUITE("Authentication cache")
{
    TEST_CASE("Setup")
    {
        std::filesystem::remove(DATABASE_PATH);
        CHECK(database.open(DATABASE_PATH));
    }

    TEST_CASE("Standard operations")
    {
        ChargePointConfigStub    cp_config;
        OcppConfigStub           ocpp_config;
        GenericMessagesConverter msg_converter;
        MessageDispatcherStub    msg_dispatcher;

        cp_config.setConfigValue("AuthentCacheMaxEntriesCount", "5");
        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "true");

        AuthentCache cache(cp_config, ocpp_config, database, msg_converter, msg_dispatcher);

        // Check register to message handler
        CHECK(msg_dispatcher.hasHandler(CLEAR_CACHE_ACTION));

        // Check empty and unknown
        IdTagInfo tag_info_r;
        IdTagInfo tag_info_w;
        CHECK_FALSE(cache.check("", tag_info_r));
        CHECK_FALSE(cache.check("TAG1", tag_info_r));

        // Check entry status
        IdToken parent_tag_1;
        parent_tag_1.assign("PARENT_TAG_1");

        // Check entry without expiry and parent
        tag_info_w.status      = AuthorizationStatus::Blocked;
        tag_info_w.parentIdTag = parent_tag_1;
        tag_info_w.expiryDate  = DateTime(DateTime::now().timestamp() + 1000);
        cache.update("TAG1", tag_info_w);
        CHECK(cache.check("TAG1", tag_info_r));
        CHECK_EQ(tag_info_r.status, tag_info_w.status);
        CHECK_EQ(tag_info_r.parentIdTag.value().str(), tag_info_w.parentIdTag.value().str());
        CHECK_EQ(tag_info_r.expiryDate.value().timestamp(), tag_info_w.expiryDate.value().timestamp());

        // Check entry without expiry and parent
        tag_info_w.status = AuthorizationStatus::Accepted;
        tag_info_w.parentIdTag.clear();
        tag_info_w.expiryDate.clear();
        cache.update("TAG1", tag_info_w);
        CHECK(cache.check("TAG1", tag_info_r));
        CHECK_EQ(tag_info_r.status, tag_info_w.status);
        CHECK_FALSE(tag_info_r.parentIdTag.isSet());
        CHECK_FALSE(tag_info_r.expiryDate.isSet());

        // Check cache size
        cache.update("TAG2", tag_info_w);
        CHECK(cache.check("TAG2", tag_info_r));
        CHECK_EQ(tag_info_r.status, tag_info_w.status);
        CHECK_FALSE(tag_info_r.parentIdTag.isSet());
        CHECK_FALSE(tag_info_r.expiryDate.isSet());

        cache.update("TAG3", tag_info_w);
        CHECK(cache.check("TAG3", tag_info_r));
        CHECK_EQ(tag_info_r.status, tag_info_w.status);
        CHECK_FALSE(tag_info_r.parentIdTag.isSet());
        CHECK_FALSE(tag_info_r.expiryDate.isSet());

        cache.update("TAG4", tag_info_w);
        CHECK(cache.check("TAG4", tag_info_r));
        CHECK_EQ(tag_info_r.status, tag_info_w.status);
        CHECK_FALSE(tag_info_r.parentIdTag.isSet());
        CHECK_FALSE(tag_info_r.expiryDate.isSet());

        cache.update("TAG5", tag_info_w);
        CHECK(cache.check("TAG5", tag_info_r));
        CHECK_EQ(tag_info_r.status, tag_info_w.status);
        CHECK_FALSE(tag_info_r.parentIdTag.isSet());
        CHECK_FALSE(tag_info_r.expiryDate.isSet());

        CHECK(cache.check("TAG1", tag_info_r));
        CHECK(cache.check("TAG2", tag_info_r));
        CHECK(cache.check("TAG3", tag_info_r));
        CHECK(cache.check("TAG4", tag_info_r));
        CHECK(cache.check("TAG5", tag_info_r));

        tag_info_w.status = AuthorizationStatus::ConcurrentTx;
        cache.update("TAG6", tag_info_w);
        CHECK(cache.check("TAG6", tag_info_r));
        CHECK_EQ(tag_info_r.status, AuthorizationStatus::Accepted);
        CHECK_FALSE(tag_info_r.parentIdTag.isSet());
        CHECK_FALSE(tag_info_r.expiryDate.isSet());

        CHECK_FALSE(cache.check("TAG1", tag_info_r));
        CHECK(cache.check("TAG2", tag_info_r));
        CHECK(cache.check("TAG3", tag_info_r));
        CHECK(cache.check("TAG4", tag_info_r));
        CHECK(cache.check("TAG5", tag_info_r));
        CHECK(cache.check("TAG6", tag_info_r));

        // Check expiry date
        tag_info_w.expiryDate = DateTime(DateTime::now().timestamp() - 1);
        cache.update("TAG4", tag_info_w);
        CHECK_FALSE(cache.check("TAG4", tag_info_r));

        CHECK_FALSE(cache.check("TAG1", tag_info_r));
        CHECK(cache.check("TAG2", tag_info_r));
        CHECK(cache.check("TAG3", tag_info_r));
        CHECK_FALSE(cache.check("TAG4", tag_info_r));
        CHECK(cache.check("TAG5", tag_info_r));
        CHECK(cache.check("TAG6", tag_info_r));

        // Check clear cache request
        ClearCacheReq  clear_req;
        ClearCacheConf clear_resp;
        const char*    error_code = nullptr;
        std::string    error_message;
        CHECK(cache.handleMessage(clear_req, clear_resp, error_code, error_message));
        CHECK_EQ(clear_resp.status, ClearCacheStatus::Accepted);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK(cache.handleMessage(clear_req, clear_resp, error_code, error_message));
        CHECK_EQ(clear_resp.status, ClearCacheStatus::Accepted);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());

        CHECK_FALSE(cache.check("TAG1", tag_info_r));
        CHECK_FALSE(cache.check("TAG2", tag_info_r));
        CHECK_FALSE(cache.check("TAG3", tag_info_r));
        CHECK_FALSE(cache.check("TAG4", tag_info_r));
        CHECK_FALSE(cache.check("TAG5", tag_info_r));
        CHECK_FALSE(cache.check("TAG6", tag_info_r));

        ocpp_config.setConfigValue("AuthorizationCacheEnabled", "false");

        CHECK(cache.handleMessage(clear_req, clear_resp, error_code, error_message));
        CHECK_EQ(clear_resp.status, ClearCacheStatus::Rejected);
        CHECK_EQ(error_code, nullptr);
        CHECK(error_message.empty());
    }

    TEST_CASE("Cleanup")
    {
        CHECK(database.close());
        std::filesystem::remove(DATABASE_PATH);
    }
}
