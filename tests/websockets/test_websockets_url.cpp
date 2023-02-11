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

#include "Url.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest_wrapper.h"

using namespace ocpp::websockets;

TEST_SUITE("Nominal - hostname as string")
{
    TEST_CASE("Short URL")
    {
        Url url("ftp://pif.com");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 0);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("Short URL with port")
    {
        Url url("ftp://pif.com:12345/");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("URL wth path")
    {
        Url url("ftp://pif.com/paf/pouf");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 0);
        CHECK_EQ(url.path(), "/paf/pouf");
    }

    TEST_CASE("URL with port and path")
    {
        Url url("ftp://pif.com:12345/paf/pouf/");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/paf/pouf/");
    }

    TEST_CASE("URL with username and port")
    {
        Url url("ftp://yip76-84@pif.com:12345");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "yip76-84");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("URL with username, password and port")
    {
        Url url("ftp://yip76-84:£uiU*^gh#@pif.com:12345");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "yip76-84");
        CHECK_EQ(url.password(), "£uiU*^gh#");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("URL with username, password, port and path")
    {
        Url url("ftp://yip76-84:£uiU*^gh#@pif.com:12345/paf/pouf/");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "yip76-84");
        CHECK_EQ(url.password(), "£uiU*^gh#");
        CHECK_EQ(url.address(), "pif.com");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/paf/pouf/");
    }

    TEST_CASE("Copy constructor")
    {
        Url url1("ftp://yip76-84:£uiU*^gh#@pif.com:12345/paf/pouf/");
        Url url2(url1);

        CHECK_EQ(url1.isValid(), url2.isValid());
        CHECK_EQ(url1.protocol(), url2.protocol());
        CHECK_EQ(url1.username(), url2.username());
        CHECK_EQ(url1.password(), url2.password());
        CHECK_EQ(url1.address(), url2.address());
        CHECK_EQ(url1.port(), url2.port());
        CHECK_EQ(url1.path(), url2.path());
    }

    TEST_CASE("Copy operator")
    {
        Url url1("ftp://yip76-84:£uiU*^gh#@pif.com:12345/paf/pouf/");
        Url url2("ftps://yip76-84:£uiU*^gh#@pif.com:12345/paf/pouf/");

        url2 = url1;

        CHECK_EQ(url1.isValid(), url2.isValid());
        CHECK_EQ(url1.protocol(), url2.protocol());
        CHECK_EQ(url1.username(), url2.username());
        CHECK_EQ(url1.password(), url2.password());
        CHECK_EQ(url1.address(), url2.address());
        CHECK_EQ(url1.port(), url2.port());
        CHECK_EQ(url1.path(), url2.path());
    }
}

TEST_SUITE("Nominal - hostname as IP address")
{
    TEST_CASE("Short URL")
    {
        Url url("ftp://10.189.70.3");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 0);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("Short URL with port")
    {
        Url url("ftp://10.189.70.3:12345/");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("URL wth path")
    {
        Url url("ftp://10.189.70.3/paf/pouf");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 0);
        CHECK_EQ(url.path(), "/paf/pouf");
    }

    TEST_CASE("URL with port and path")
    {
        Url url("ftp://10.189.70.3:12345/paf/pouf/");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/paf/pouf/");
    }

    TEST_CASE("URL with username and port")
    {
        Url url("ftp://yip76-84@10.189.70.3:12345");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "yip76-84");
        CHECK_EQ(url.password(), "");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("URL with username, password and port")
    {
        Url url("ftp://yip76-84:£uiU*^gh#@10.189.70.3:12345");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "yip76-84");
        CHECK_EQ(url.password(), "£uiU*^gh#");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/");
    }

    TEST_CASE("URL with username, password, port and path")
    {
        Url url("ftp://yip76-84:£uiU*^gh#@10.189.70.3:12345/paf/pouf/");

        CHECK(url.isValid());
        CHECK_EQ(url.protocol(), "ftp");
        CHECK_EQ(url.username(), "yip76-84");
        CHECK_EQ(url.password(), "£uiU*^gh#");
        CHECK_EQ(url.address(), "10.189.70.3");
        CHECK_EQ(url.port(), 12345);
        CHECK_EQ(url.path(), "/paf/pouf/");
    }

    TEST_CASE("Copy constructor")
    {
        Url url1("ftp://yip76-84:£uiU*^gh#@10.189.70.3:12345/paf/pouf/");
        Url url2(url1);

        CHECK_EQ(url1.isValid(), url2.isValid());
        CHECK_EQ(url1.protocol(), url2.protocol());
        CHECK_EQ(url1.username(), url2.username());
        CHECK_EQ(url1.password(), url2.password());
        CHECK_EQ(url1.address(), url2.address());
        CHECK_EQ(url1.port(), url2.port());
        CHECK_EQ(url1.path(), url2.path());
    }

    TEST_CASE("Copy operator")
    {
        Url url1("ftp://yip76-84:£uiU*^gh#@10.189.70.3:12345/paf/pouf/");
        Url url2("ftps://yip76-84:£uiU*^gh#@10.189.70.3:12345/paf/pouf/");

        url2 = url1;

        CHECK_EQ(url1.isValid(), url2.isValid());
        CHECK_EQ(url1.protocol(), url2.protocol());
        CHECK_EQ(url1.username(), url2.username());
        CHECK_EQ(url1.password(), url2.password());
        CHECK_EQ(url1.address(), url2.address());
        CHECK_EQ(url1.port(), url2.port());
        CHECK_EQ(url1.path(), url2.path());
    }
}

TEST_SUITE("Errors")
{
    TEST_CASE("Default constructor")
    {
        Url url;

        CHECK_FALSE(url.isValid());
    }

    TEST_CASE("Invalid protocol")
    {
        Url url("ftp//pif.com");

        CHECK_FALSE(url.isValid());
    }

    TEST_CASE("Missing protocol")
    {
        Url url("pif.com");

        CHECK_FALSE(url.isValid());
    }

    TEST_CASE("Invalid port - not a number")
    {
        Url url("ftp://pif.com:abcd/");

        CHECK_FALSE(url.isValid());
    }

    TEST_CASE("Invalid port - range min")
    {
        Url url("ftp://pif.com:0/");

        CHECK_FALSE(url.isValid());
    }

    TEST_CASE("Invalid port - range max")
    {
        Url url("ftp://pif.com:65536/");

        CHECK_FALSE(url.isValid());
    }
}
