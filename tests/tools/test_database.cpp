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

#include "Database.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest_wrapper.h"

#include <filesystem>
#include <limits>

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

    TEST_CASE("Standard operations")
    {
        Database db;

        auto query = db.query("SELECT * FROM TestTable;");
        CHECK_EQ(query.get(), nullptr);
        CHECK(db.open(test_database_path.string()));

        query = db.query("SELECT * FROM TestTable;");
        CHECK_EQ(query.get(), nullptr);
        query = db.query("CREATE TABLE TestTable ("
                         "[IntField]	    INTEGER,"
                         "[TextField]	    VARCHAR(20),"
                         "[FloatField]	    REAL,"
                         "[BoolFieldFalse]  BOOLEAN,"
                         "[BoolFieldTrue]   BOOLEAN,"
                         "[Int64Field]      BIGINT,"
                         "PRIMARY KEY([IntField] AUTOINCREMENT));");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->exec());
        CHECK_FALSE(query->hasRows());

        query = db.query("SELECT * FROM TestTable;");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->exec());
        CHECK_FALSE(query->hasRows());

        for (int i = 0; i < 10; i++)
        {
            query = db.query("INSERT INTO TestTable VALUES(NULL, ?, ?, ?, ?, ?);");
            CHECK_NE(query.get(), nullptr);
            CHECK(query->bind(0, "Pif paf pouf"));
            CHECK(query->bind(1, 123.456));
            CHECK(query->bind(2, false));
            CHECK(query->bind(3, true));
            CHECK(query->bind(4, std::numeric_limits<int64_t>::max()));
            CHECK(query->exec());
            CHECK_FALSE(query->hasRows());
        }

        query = db.query("SELECT * FROM TestTable;");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->exec());
        CHECK(query->hasRows());

        unsigned int count = 0;
        do
        {
            count++;
            CHECK_EQ(query->getUInt32(0), count);
            CHECK_EQ(query->getString(1), "Pif paf pouf");
            CHECK_EQ(query->getFloat(2), 123.456);
            CHECK_EQ(query->getBool(3), false);
            CHECK_EQ(query->getBool(4), true);
            CHECK_EQ(query->getInt64(5), std::numeric_limits<int64_t>::max());
        } while (query->next());
        CHECK_EQ(count, 10u);

        CHECK(query->exec());
        CHECK(query->hasRows());

        count = 0;
        do
        {
            count++;
            CHECK_EQ(query->getUInt32(0), count);
            CHECK_EQ(query->getString(1), "Pif paf pouf");
            CHECK_EQ(query->getFloat(2), 123.456);
            CHECK_EQ(query->getBool(3), false);
            CHECK_EQ(query->getBool(4), true);
            CHECK_EQ(query->getInt64(5), std::numeric_limits<int64_t>::max());
        } while (query->next());
        CHECK_EQ(count, 10u);

        query = db.query("SELECT * FROM TestTable WHERE [IntField]>? AND [FloatField]=?;");
        CHECK_NE(query.get(), nullptr);
        CHECK(query->bind(0, 5));
        CHECK(query->bind(1, 123.456));
        CHECK(query->exec());
        CHECK(query->hasRows());

        count = 0;
        do
        {
            count++;
            CHECK_EQ(query->getUInt32(0), 5u + count);
            CHECK_EQ(query->getString(1), "Pif paf pouf");
            CHECK_EQ(query->getFloat(2), 123.456);
            CHECK_EQ(query->getBool(3), false);
            CHECK_EQ(query->getBool(4), true);
            CHECK_EQ(query->getInt64(5), std::numeric_limits<int64_t>::max());
        } while (query->next());
        CHECK_EQ(count, 5u);

        CHECK(db.close());
        query = db.query("SELECT * FROM TestTable;");
        CHECK_EQ(query.get(), nullptr);
    }

    TEST_CASE("Cleanup")
    {
        std::filesystem::remove(test_database_path);
    }
}
