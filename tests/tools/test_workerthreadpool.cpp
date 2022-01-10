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

#include "WorkerThreadPool.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace ocpp::helpers;

TEST_SUITE("WorkerThreadPool class test suite")
{
    TEST_CASE("Thread management")
    {
        WorkerThreadPool worker_thread_pool(3);

        unsigned int jobs_done = 0;

        std::mutex              end_job1_mutex;
        std::condition_variable end_job1_var;
        bool                    end_job_1 = false;
        auto                    job1      = [&jobs_done, &end_job1_mutex, &end_job1_var, &end_job_1]
        {
            std::unique_lock<std::mutex> lock(end_job1_mutex);
            end_job1_var.wait_for(lock, std::chrono::milliseconds(1000u), [&end_job_1] { return end_job_1; });
            jobs_done++;
        };

        worker_thread_pool.run<void>(job1);
        Waiter<void> waiter1 = worker_thread_pool.run<void>(job1);

        auto job2 = [&jobs_done]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(25u));
            jobs_done++;
        };
        worker_thread_pool.run<void>(job2);
        worker_thread_pool.run<void>(job2);
        worker_thread_pool.run<void>(job2);
        worker_thread_pool.run<void>(job2);
        Waiter<void> waiter2 = worker_thread_pool.run<void>(job2);

        CHECK(waiter2.wait());
        CHECK_EQ(jobs_done, 5u);

        end_job_1 = true;
        end_job1_var.notify_all();

        CHECK(waiter1.wait());
        CHECK_EQ(jobs_done, 7u);
    }

    TEST_CASE("Jobs without return value")
    {
        WorkerThreadPool worker_thread_pool(3);

        auto job1 = [] { std::this_thread::sleep_for(std::chrono::milliseconds(20)); };
        auto job2 = [] { return; };
        auto job3 = []
        {
            throw std::exception();
            return false;
        };

        Waiter<void> waiter1 = worker_thread_pool.run<void>(job1);
        Waiter<void> waiter2 = worker_thread_pool.run<void>(job2);
        Waiter<void> waiter3 = worker_thread_pool.run<void>(job3);

        CHECK(waiter1.wait());
        CHECK(waiter2.wait());
        CHECK(waiter3.wait());

        CHECK(waiter1.success());
        CHECK(waiter2.success());
        CHECK_FALSE(waiter3.success());
    }

    TEST_CASE("Jobs with return value")
    {
        WorkerThreadPool worker_thread_pool(3);

        auto job1 = []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            return false;
        };
        auto job2 = [] { return true; };
        auto job3 = [] { return false; };
        auto job4 = []
        {
            throw std::exception();
            return false;
        };

        Waiter<bool> waiter1 = worker_thread_pool.run<bool>(job1);
        Waiter<bool> waiter2 = worker_thread_pool.run<bool>(job2);
        Waiter<bool> waiter3 = worker_thread_pool.run<bool>(job3);
        Waiter<bool> waiter4 = worker_thread_pool.run<bool>(job4);

        CHECK(waiter1.wait());
        CHECK(waiter2.wait());
        CHECK(waiter3.wait());
        CHECK(waiter4.wait());

        CHECK(waiter1.success());
        CHECK_FALSE(waiter1.value());
        CHECK(waiter2.success());
        CHECK(waiter2.value());
        CHECK(waiter3.success());
        CHECK_FALSE(waiter3.value());
        CHECK_FALSE(waiter4.success());

        auto job5 = []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            return std::string("Pif");
        };
        auto job6 = [] { return std::string("Paf"); };
        auto job7 = [] { return std::string("Pouf"); };
        auto job8 = []
        {
            throw std::exception();
            return std::string("Poum");
        };

        Waiter<std::string> waiter5 = worker_thread_pool.run<std::string>(job5);
        Waiter<std::string> waiter6 = worker_thread_pool.run<std::string>(job6);
        Waiter<std::string> waiter7 = worker_thread_pool.run<std::string>(job7);
        Waiter<std::string> waiter8 = worker_thread_pool.run<std::string>(job8);

        CHECK(waiter5.wait());
        CHECK(waiter6.wait());
        CHECK(waiter7.wait());
        CHECK(waiter8.wait());

        CHECK(waiter5.success());
        CHECK_EQ(waiter5.value(), "Pif");
        CHECK(waiter6.success());
        CHECK_EQ(waiter6.value(), "Paf");
        CHECK(waiter7.success());
        CHECK_EQ(waiter7.value(), "Pouf");
        CHECK_FALSE(waiter8.success());
    }
}
