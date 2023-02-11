/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License version 2.1
as published by the Free Software Foundation.

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
#include "Logger.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest_wrapper.h"

#include <string>

using namespace ocpp::log;

#ifdef EXTERNAL_LOGGER

TEST_SUITE("External logger class test suite")
{
    TEST_CASE("Custom log function")
    {
        unsigned int log_level = 0;
        std::string  log_str;
        auto         log_function = [&](unsigned int level, const std::string& log)
        {
            log_level = level;
            log_str   = log;
        };

        ExtLogger::registerLogFunction(log_function);

        LOG_COM << "This one will be saved!";
        CHECK_EQ(log_level, 1);
        CHECK_NE(log_str.find("This one will be saved!"), std::string::npos);

        LOG_DEBUG << "This one too!";
        CHECK_EQ(log_level, 0);
        CHECK_NE(log_str.find("This one too!"), std::string::npos);

        LOG_INFO << "This one either!";
        CHECK_EQ(log_level, 2);
        CHECK_NE(log_str.find("This one either!"), std::string::npos);

        LOG_WARNING << "And also this one!";
        CHECK_EQ(log_level, 3);
        CHECK_NE(log_str.find("And also this one!"), std::string::npos);

        LOG_ERROR << "This is the last one saved!";
        CHECK_EQ(log_level, 4);
        CHECK_NE(log_str.find("This is the last one saved!"), std::string::npos);
    }
}

#else // EXTERNAL_LOGGER

#include "Database.h"
#include "LogDatabase.h"

#include <filesystem>

using namespace ocpp::database;

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
        CHECK(db.open(test_database_path.string()));

        LogDatabase log_db1(db, "logs_1", 10u);
        for (size_t i = 0; i < 20; i++)
        {
            log_db1.log(std::chrono::steady_clock::now().time_since_epoch().count(),
                        static_cast<unsigned int>(1u + i),
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
        CHECK(db.open(test_database_path.string()));

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
        CHECK(db.open(test_database_path.string()));

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

#endif // EXTERNAL_LOGGER
