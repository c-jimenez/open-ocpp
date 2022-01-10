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

#ifdef LOG_LEVEL
#undef LOG_LEVEL
#endif
#define LOG_LEVEL 0 // Enable all log levels
#include "Database.h"
#include "LogDatabase.h"
#include "Logger.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <filesystem>
#include <string>

using namespace ocpp::database;
using namespace ocpp::log;

std::filesystem::path test_database_path;

TEST_SUITE("Database class test suite")
{
    TEST_CASE("Setup")
    {
        test_database_path = std::filesystem::temp_directory_path();
        test_database_path.append("test.db");
        std::filesystem::remove(test_database_path);
    }

    TEST_CASE("Log database")
    {
        Database db;
        CHECK(db.open(test_database_path));

        LogDatabase log_db1(db, "logs_1", 10u);
        for (size_t i = 0; i < 20; i++)
        {
            log_db1.log(std::chrono::steady_clock::now().time_since_epoch().count(),
                        1 + i,
                        "file.cpp:" + std::to_string(i),
                        "My log! " + std::to_string(i));
        }

        auto query = db.query("SELECT * FROM logs_1;");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->exec());
        CHECK(query->hasRows());

        unsigned int count = 0;
        unsigned int start = 10;
        do
        {
            count++;
            start++;
            CHECK_EQ(query->getUInt32(0), start);
            CHECK_EQ(query->getUInt32(2), start);
            CHECK_EQ(query->getString(3), "file.cpp:" + std::to_string(start - 1));
            CHECK_EQ(query->getString(4), "My log! " + std::to_string(start - 1));
        } while (query->next());
        CHECK_EQ(count, 10u);
    }

    TEST_CASE("Default logger")
    {
        Database db;
        CHECK(db.open(test_database_path));

        LOG_INFO << "This log won't be saved!";

        Logger::registerDefaultLogger(db, 20u);

        LOG_COM << "This one will be saved!";
        LOG_DEBUG << "This one too!";
        LOG_INFO << "This one either!";
        LOG_WARNING << "And also this one!";
        LOG_ERROR << "This is the last one saved!";

        auto query = db.query("SELECT * FROM " DEFAULT_LOG_NAME " ORDER BY id ASC;");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->exec());
        CHECK(query->hasRows());

        CHECK_EQ(query->getUInt32(2), 1);
        CHECK_EQ(query->getString(4), "This one will be saved!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 0);
        CHECK_EQ(query->getString(4), "This one too!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 2);
        CHECK_EQ(query->getString(4), "This one either!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 3);
        CHECK_EQ(query->getString(4), "And also this one!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 4);
        CHECK_EQ(query->getString(4), "This is the last one saved!");
        CHECK_FALSE(query->next());
    }

    TEST_CASE("Custom logger")
    {
        Database db;
        CHECK(db.open(test_database_path));

        LOG_INFO << "This log won't be saved!";

        Logger::registerLogger(db, "MyLogs", 20u);

        LOG_ERROR2("MyLogs") << "This one will be saved!";
        LOG_WARNING2("MyLogs") << "This one too!";
        LOG_INFO2("MyLogs") << "This one either!";
        LOG_DEBUG2("MyLogs") << "And also this one!";
        LOG_COM2("MyLogs") << "This is the last one saved!";

        auto query = db.query("SELECT * FROM MyLogs ORDER BY id ASC;");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->exec());
        CHECK(query->hasRows());

        CHECK_EQ(query->getUInt32(2), 4);
        CHECK_EQ(query->getString(4), "This one will be saved!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 3);
        CHECK_EQ(query->getString(4), "This one too!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 2);
        CHECK_EQ(query->getString(4), "This one either!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 0);
        CHECK_EQ(query->getString(4), "And also this one!");
        CHECK(query->next());

        CHECK_EQ(query->getUInt32(2), 1);
        CHECK_EQ(query->getString(4), "This is the last one saved!");
        CHECK_FALSE(query->next());
    }

    TEST_CASE("Cleanup") { std::filesystem::remove(test_database_path); }
}
