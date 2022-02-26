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

#include "Timer.h"
#include "TimerPool.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <memory>

using namespace ocpp::helpers;

TEST_SUITE("Timers class test suite")
{
    TEST_CASE("Standard operations - multi shots")
    {
        TimerPool              pool;
        std::unique_ptr<Timer> timer(pool.createTimer("test_timer"));
        unsigned int           calls    = 0;
        auto                   callback = [&calls] { calls++; };
        timer->setCallback(callback);

        CHECK_FALSE(timer->isStarted());
        CHECK_FALSE(timer->stop());
        CHECK(timer->start(std::chrono::milliseconds(250u)));
        CHECK(timer->isStarted());
        CHECK_EQ(timer->getInterval(), std::chrono::milliseconds(250u));
        std::this_thread::sleep_for(std::chrono::milliseconds(800u));
        CHECK_EQ(calls, 3u);
        std::this_thread::sleep_for(std::chrono::milliseconds(250u));
        CHECK_EQ(calls, 4u);
        CHECK(timer->stop());
        CHECK_FALSE(timer->isStarted());
        CHECK(timer->start(std::chrono::milliseconds(10u)));
        CHECK(timer->stop());
        CHECK_EQ(pool.getTimer("test_timer"), timer.get());
    }

    TEST_CASE("Standard operations - single shot")
    {
        TimerPool              pool;
        std::unique_ptr<Timer> timer(pool.createTimer());
        unsigned int           calls    = 0;
        auto                   callback = [&calls] { calls++; };
        timer->setCallback(callback);

        CHECK_FALSE(timer->isStarted());
        CHECK_FALSE(timer->stop());
        CHECK(timer->start(std::chrono::milliseconds(250u), true));
        CHECK(timer->isStarted());
        CHECK_EQ(timer->getInterval(), std::chrono::milliseconds(250u));
        std::this_thread::sleep_for(std::chrono::milliseconds(600u));
        CHECK_EQ(calls, 1u);
        CHECK_FALSE(timer->isStarted());
        CHECK_FALSE(timer->stop());
        CHECK(timer->start(std::chrono::milliseconds(250u), true));
        CHECK(timer->isStarted());
        CHECK(timer->stop());
        CHECK_EQ(calls, 1u);
        CHECK_FALSE(timer->isStarted());
        CHECK(timer->start(std::chrono::milliseconds(250u), true));
        CHECK(timer->isStarted());
        std::this_thread::sleep_for(std::chrono::milliseconds(300u));
        CHECK_EQ(calls, 2u);
        CHECK_FALSE(timer->isStarted());
        CHECK_FALSE(timer->stop());
    }

    TEST_CASE("Standard operations - stop & restart from within the callback")
    {
        TimerPool              pool;
        std::unique_ptr<Timer> timer(pool.createTimer());
        unsigned int           calls         = 0;
        auto                   callback_stop = [&calls, &timer]
        {
            calls++;
            if (calls == 2u)
            {
                timer->stop();
            }
        };

        timer->setCallback(callback_stop);
        CHECK(timer->start(std::chrono::milliseconds(250u)));
        CHECK(timer->isStarted());
        CHECK_EQ(timer->getInterval(), std::chrono::milliseconds(250u));
        std::this_thread::sleep_for(std::chrono::milliseconds(800u));
        CHECK_EQ(calls, 2u);
        CHECK_FALSE(timer->isStarted());

        auto callback_restart = [&calls, &timer]
        {
            calls++;
            if (calls == 2u)
            {
                timer->restart(std::chrono::milliseconds(500u));
            }
            if (calls == 3u)
            {
                timer->stop();
            }
        };
        calls = 0;
        timer->setCallback(callback_restart);
        CHECK(timer->start(std::chrono::milliseconds(250u)));
        CHECK(timer->isStarted());
        CHECK_EQ(timer->getInterval(), std::chrono::milliseconds(250u));
        std::this_thread::sleep_for(std::chrono::milliseconds(800u));
        CHECK_EQ(calls, 2u);
        CHECK(timer->isStarted());
        CHECK_EQ(timer->getInterval(), std::chrono::milliseconds(500u));
        std::this_thread::sleep_for(std::chrono::milliseconds(250u));
        CHECK_EQ(calls, 3u);
        CHECK_FALSE(timer->isStarted());
    }

    TEST_CASE("Standard operations - multiple timers")
    {
        TimerPool              pool;
        std::unique_ptr<Timer> timer1(pool.createTimer());
        std::unique_ptr<Timer> timer2(pool.createTimer());
        std::unique_ptr<Timer> timer3(pool.createTimer());
        unsigned int           calls1    = 0;
        unsigned int           calls2    = 0;
        unsigned int           calls3    = 0;
        auto                   callback1 = [&calls1] { calls1++; };
        auto                   callback2 = [&calls2] { calls2++; };
        auto                   callback3 = [&calls3] { calls3++; };
        timer1->setCallback(callback1);
        timer2->setCallback(callback2);
        timer3->setCallback(callback3);

        CHECK(timer1->start(std::chrono::milliseconds(200u)));
        CHECK(timer2->start(std::chrono::milliseconds(300u)));
        CHECK(timer3->start(std::chrono::milliseconds(100u)));

        std::this_thread::sleep_for(std::chrono::milliseconds(110u));

        CHECK_EQ(calls1, 0u);
        CHECK_EQ(calls2, 0u);
        CHECK_EQ(calls3, 1u);

        std::this_thread::sleep_for(std::chrono::milliseconds(110u));

        CHECK_EQ(calls1, 1u);
        CHECK_EQ(calls2, 0u);
        CHECK_EQ(calls3, 2u);

        std::this_thread::sleep_for(std::chrono::milliseconds(110u));

        CHECK_EQ(calls1, 1u);
        CHECK_EQ(calls2, 1u);
        CHECK_EQ(calls3, 3u);

        std::this_thread::sleep_for(std::chrono::milliseconds(110u));

        CHECK_EQ(calls1, 2u);
        CHECK_EQ(calls2, 1u);
        CHECK_EQ(calls3, 4u);

        std::this_thread::sleep_for(std::chrono::milliseconds(110u));

        CHECK_EQ(calls1, 2u);
        CHECK_EQ(calls2, 1u);
        CHECK_EQ(calls3, 5u);

        std::this_thread::sleep_for(std::chrono::milliseconds(110u));

        CHECK_EQ(calls1, 3u);
        CHECK_EQ(calls2, 2u);
        CHECK_EQ(calls3, 6u);
    }
}
