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

#include "IniFile.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace ocpp::helpers;

TEST_SUITE("IniFile class test suite")
{
    TEST_CASE("Create from empty")
    {
        IniFile ini_file;

        CHECK_EQ(ini_file.sections().size(), 0u);

        CHECK_FALSE(ini_file.load("not_existing.ini"));

        CHECK_EQ(ini_file.sections().size(), 0u);

        ini_file.set("First section", "My param 1", IniFile::Value(-24));
        ini_file.set("First section", "My param 2", IniFile::Value(true));
        ini_file.set("Second section", "My param 1", IniFile::Value(567.89));

        auto sections = ini_file.sections();
        CHECK_EQ(sections.size(), 2u);

        auto params = ini_file["First section"];
        CHECK_EQ(params.size(), 2u);

        params = ini_file["Second section"];
        CHECK_EQ(params.size(), 1u);

        CHECK_EQ(ini_file.get("First section", "My param 1").toInt(), -24);
        CHECK_EQ(ini_file.get("First section", "My param 2").toBool(), true);
        CHECK_EQ(ini_file.get("Second section", "My param 1").toFloat(), 567.89);

        CHECK(ini_file.store("/tmp/test.ini"));

        ini_file.clear();
        CHECK_EQ(ini_file.sections().size(), 0u);

        CHECK(ini_file.load("/tmp/test.ini"));

        sections = ini_file.sections();
        CHECK_EQ(sections.size(), 2u);

        params = ini_file["First section"];
        CHECK_EQ(params.size(), 2u);

        params = ini_file["Second section"];
        CHECK_EQ(params.size(), 1u);

        CHECK_EQ(ini_file.get("First section", "My param 1").toInt(), -24);
        CHECK_EQ(ini_file.get("First section", "My param 2").toBool(), true);
        CHECK_EQ(ini_file.get("Second section", "My param 1").toFloat(), 567.89);

        ini_file.set("Third section", "My param 1", IniFile::Value(6789u));

        IniFile ini_file2;
        CHECK(ini_file2.load("/tmp/test.ini", false));

        sections = ini_file2.sections();
        CHECK_EQ(sections.size(), 3u);

        params = ini_file2["First section"];
        CHECK_EQ(params.size(), 2u);

        params = ini_file2["Second section"];
        CHECK_EQ(params.size(), 1u);

        params = ini_file2["Third section"];
        CHECK_EQ(params.size(), 1u);

        CHECK_EQ(ini_file2.get("First section", "My param 1").toInt(), -24);
        CHECK_EQ(ini_file2.get("First section", "My param 2").toBool(), true);
        CHECK_EQ(ini_file2.get("Second section", "My param 1").toFloat(), 567.89);
        CHECK_EQ(ini_file2.get("Third section", "My param 1").toUInt(), 6789u);

        ini_file2.set("Third section", "My param 2", IniFile::Value("Ho ho ho!"));

        IniFile ini_file3;
        CHECK(ini_file3.load("/tmp/test.ini", false));

        sections = ini_file3.sections();
        CHECK_EQ(sections.size(), 3u);

        params = ini_file3["First section"];
        CHECK_EQ(params.size(), 2u);

        params = ini_file3["Second section"];
        CHECK_EQ(params.size(), 1u);

        params = ini_file3["Third section"];
        CHECK_EQ(params.size(), 1u);

        CHECK_EQ(ini_file3.get("First section", "My param 1").toInt(), -24);
        CHECK_EQ(ini_file3.get("First section", "My param 2").toBool(), true);
        CHECK_EQ(ini_file3.get("Second section", "My param 1").toFloat(), 567.89);
        CHECK_EQ(ini_file3.get("Third section", "My param 1").toUInt(), 6789u);
    }

    TEST_CASE("Values")
    {
        IniFile::Value val1("12345");
        CHECK_FALSE(val1.isEmpty());
        CHECK(val1.isInt());
        CHECK(val1.isUInt());
        CHECK(val1.isFloat());
        CHECK_FALSE(val1.isBool());
        CHECK_EQ(val1.toInt(), 12345);
        CHECK_EQ(val1.toUInt(), 12345u);
        CHECK_EQ(val1.toFloat(), 12345.);
        CHECK_EQ(val1.toString(), "12345");

        val1 = IniFile::Value("-12345");
        CHECK_FALSE(val1.isEmpty());
        CHECK(val1.isInt());
        CHECK_FALSE(val1.isUInt());
        CHECK(val1.isFloat());
        CHECK_FALSE(val1.isBool());
        CHECK_EQ(val1.toInt(), -12345);
        CHECK_EQ(val1.toFloat(), -12345.);
        CHECK_EQ(val1.toString(), "-12345");

        val1 = IniFile::Value("12345.6789");
        CHECK_FALSE(val1.isEmpty());
        CHECK_FALSE(val1.isInt());
        CHECK_FALSE(val1.isUInt());
        CHECK(val1.isFloat());
        CHECK_FALSE(val1.isBool());
        CHECK_EQ(val1.toFloat(), 12345.6789);
        CHECK_EQ(val1.toString(), "12345.6789");

        val1 = "true";
        CHECK_FALSE(val1.isEmpty());
        CHECK_FALSE(val1.isInt());
        CHECK_FALSE(val1.isUInt());
        CHECK_FALSE(val1.isFloat());
        CHECK(val1.isBool());
        CHECK_EQ(val1.toBool(), true);
        CHECK_EQ(val1.toString(), "true");

        val1 = "false";
        CHECK_FALSE(val1.isEmpty());
        CHECK_FALSE(val1.isInt());
        CHECK_FALSE(val1.isUInt());
        CHECK_FALSE(val1.isFloat());
        CHECK(val1.isBool());
        CHECK_EQ(val1.toBool(), false);
        CHECK_EQ(val1.toString(), "false");

        val1 = IniFile::Value();
        CHECK(val1.isEmpty());
        CHECK_FALSE(val1.isInt());
        CHECK_FALSE(val1.isUInt());
        CHECK_FALSE(val1.isFloat());
        CHECK_FALSE(val1.isBool());
        CHECK_EQ(val1.toString(), "");

        val1 = "56a7.dmakfza";
        CHECK_FALSE(val1.isEmpty());
        CHECK_FALSE(val1.isInt());
        CHECK_FALSE(val1.isUInt());
        CHECK_FALSE(val1.isFloat());
        CHECK_FALSE(val1.isBool());
        CHECK_EQ(val1.toString(), "56a7.dmakfza");
    }
}
