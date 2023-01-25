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

#include "Queue.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest_wrapper.h"

#include <thread>

using namespace ocpp::helpers;

TEST_SUITE("Queue class test suite - Nominal")
{
    TEST_CASE("Standard operations")
    {
        Queue<size_t, 10> queue;

        CHECK_EQ(queue.size(), 10);
        CHECK_EQ(queue.count(), 0);
        CHECK(queue.empty());
        CHECK_FALSE(queue.full());

        for (size_t i = 0; i < queue.size(); i++)
        {
            CHECK(queue.push(i));
            CHECK_EQ(queue.count(), (i + 1));
        }
        CHECK_FALSE(queue.push(55u));
        CHECK_FALSE(queue.empty());
        CHECK(queue.full());

        size_t val = 0;
        for (size_t i = 0; i < queue.size(); i++)
        {
            CHECK(queue.pop(val, 0));
            CHECK_EQ(val, i);
            CHECK_EQ(queue.count(), (queue.size() - (i + 1)));
        }
        CHECK_FALSE(queue.pop(val, 0));
        CHECK(queue.empty());
        CHECK_FALSE(queue.full());

        CHECK(queue.push(45u));
        CHECK(queue.push(890u));
        CHECK(queue.push(3456u));
        CHECK_EQ(queue.count(), 3u);
        queue.clear();
        CHECK_EQ(queue.count(), 0u);
        CHECK(queue.empty());
    }

    TEST_CASE("Timeout management")
    {
        Queue<int> queue;

        int val = 0;
        CHECK_FALSE(queue.pop(val, 200u));

        std::thread push_thread(
            [&queue]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                queue.push(12345);
            });
        push_thread.detach();

        CHECK(queue.pop(val, 2000u));
        CHECK_EQ(val, 12345);

        CHECK_FALSE(queue.pop(val, 200u));

        std::thread cancel_thread(
            [&queue]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                queue.setEnable(false);
            });
        cancel_thread.detach();
        CHECK_FALSE(queue.pop(val));

        queue.setEnable(true);
        queue.push(12345);
        CHECK(queue.pop(val));
        CHECK_EQ(val, 12345);
    }

    TEST_CASE("O length queue")
    {
        Queue<int, 0> queue;

        CHECK_EQ(queue.size(), 0);
        CHECK_EQ(queue.count(), 0);
        CHECK(queue.empty());
        CHECK(queue.full());
        CHECK_FALSE(queue.push(1));

        int val = 0;
        CHECK_FALSE(queue.pop(val, 0));
    }
}
