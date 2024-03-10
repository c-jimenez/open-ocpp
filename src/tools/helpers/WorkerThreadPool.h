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

#ifndef OPENOCPP_WORKERTHREADPOOL_H
#define OPENOCPP_WORKERTHREADPOOL_H

#include "Queue.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

namespace ocpp
{
namespace helpers
{

/** @brief Interface for job for a worker thread implementations */
class IJob
{
  public:
    /** @brief Destructor */
    virtual ~IJob() { }
    /** @brief Run the job */
    virtual void run() = 0;
};

/** @brief Base class for a job for a worker thread */
template <typename ReturnType>
class JobBase : public IJob
{
  public:
    /** @brief Constructor */
    JobBase(const std::function<ReturnType()>& func) : success(true), end_of_job_mutex(), end_of_job_var(), end(false), function(func) { }

    /** @brief Destructor */
    virtual ~JobBase() { }

    /** @brief Indicate the job did execute without uncatched exception */
    bool success;
    /** @brief Mutex for end of job synchronization */
    std::mutex end_of_job_mutex;
    /** @brief Condition variable for end of job synchronization */
    std::condition_variable end_of_job_var;
    /** @brief Indicate end of job */
    std::atomic<bool> end;
    /** @brief Function to execute */
    std::function<ReturnType()> function;
};

/** @brief Job for a worker thread */
template <typename ReturnType>
class Job : public JobBase<ReturnType>
{
    // Needed to protect instanciation outside of WorkerThreadPool class
    friend class WorkerThreadPool;

  public:
    /** @brief Destructor */
    virtual ~Job() { }

    /** @brief Run the job */
    void run() override
    {
        try
        {
            // Execute the job and store the returned value
            ret_value = this->function();
        }
        catch (...)
        {
            this->success = false;
        }

        // Notify end of job
        {
            std::lock_guard<std::mutex> lock(this->end_of_job_mutex);
            this->end = true;
        }
        this->end_of_job_var.notify_all();
    }

    /** @brief Returned value */
    ReturnType ret_value;

  private:
    /** @brief Constructor */
    Job(const std::function<ReturnType()>& func) : JobBase<ReturnType>(func), ret_value() { }
};

/** @brief Job for a worker thread without return value */
template <>
class Job<void> : public JobBase<void>
{
    // Needed to protect instanciation outside of WorkerThreadPool class
    friend class WorkerThreadPool;

  public:
    /** @brief Destructor */
    virtual ~Job() { }

    /** @brief Run the job */
    void run() override
    {
        try
        {
            // Execute the job and store the returned value
            this->function();
        }
        catch (...)
        {
            this->success = false;
        }

        // Notify end of job
        {
            std::lock_guard<std::mutex> lock(this->end_of_job_mutex);
            this->end = true;
        }
        this->end_of_job_var.notify_all();
    }

  private:
    /** @brief Constructor */
    Job(const std::function<void()>& func) : JobBase<void>(func) { }
};

/** @brief Allow to wait on asynchronous execution of a function */
template <typename ReturnType>
class Waiter
{
    // Needed to protect instanciation outside of WorkerThreadPool class
    friend class WorkerThreadPool;

  public:
    /** @brief Get the returned value */
    const ReturnType& value() const { return (dynamic_cast<Job<ReturnType>*>(m_job.get()))->ret_value; }

    /** @brief Indicate the job did execute without uncatched exception */
    bool success() { return (dynamic_cast<Job<ReturnType>*>(m_job.get()))->success; }

    /** @brief Wait for completion */
    bool wait(std::chrono::milliseconds timeout = std::chrono::hours(24))
    {
        Job<ReturnType>*             job = dynamic_cast<Job<ReturnType>*>(m_job.get());
        std::unique_lock<std::mutex> lock(job->end_of_job_mutex);
        return job->end_of_job_var.wait_for(lock, timeout, [job] { return job->end.operator bool(); });
    }

  private:
    /** @brief Constructor */
    Waiter(const std::shared_ptr<IJob>& job) : m_job(job) { }

    /** @brief Associated job */
    std::shared_ptr<IJob> m_job;
};

/** @brief Allow to wait on asynchronous execution of a function withour return value */
template <>
class Waiter<void>
{
    // Needed to protect instanciation outside of WorkerThreadPool class
    friend class WorkerThreadPool;

  public:
    /** @brief Indicate the job did execute without uncatched exception */
    bool success() { return (dynamic_cast<Job<void>*>(m_job.get()))->success; }

    /** @brief Wait for completion */
    bool wait(std::chrono::milliseconds timeout = std::chrono::hours(24))
    {
        Job<void>*                   job = dynamic_cast<Job<void>*>(m_job.get());
        std::unique_lock<std::mutex> lock(job->end_of_job_mutex);
        return job->end_of_job_var.wait_for(lock, timeout, [job] { return job->end.operator bool(); });
    }

  private:
    /** @brief Constructor */
    Waiter(const std::shared_ptr<IJob>& job) : m_job(job) { }

    /** @brief Associated job */
    std::shared_ptr<IJob> m_job;
};

/** @brief Handle a pool of worker threads */
class WorkerThreadPool
{
  public:
    /**
     * @brief Constructor
     * @param thread_count Number of worker threads
     */
    WorkerThreadPool(size_t thread_count);
    /** @brief Destructor */
    virtual ~WorkerThreadPool();

    /** @brief Run a function in a worker thread */
    template <typename ReturnType>
    Waiter<ReturnType> run(std::function<ReturnType()> func)
    {
        // Create job
        std::shared_ptr<IJob> job(new Job<ReturnType>(func));

        // Add the job to the queue
        queue(job);

        // Create the waiter object
        return Waiter<ReturnType>(job);
    }

  protected:
    /** @brief Add a job to the queue */
    virtual void queue(const std::shared_ptr<IJob>& job) { m_job_queue.push(job); }

  private:
    /** @brief Indicate that the threads must stop */
    std::atomic<bool> m_stop;
    /** @brief Worker threads */
    std::vector<std::thread*> m_threads;
    /** @brief Job queue */
    Queue<std::shared_ptr<IJob>> m_job_queue;

    /** @brief Worker thread */
    void workerThread();
};

} // namespace helpers
} // namespace ocpp

#endif // OPENOCPP_WORKERTHREADPOOL_H
