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

#include "Logger.h"
#include "LogDatabase.h"

#include <iomanip>
#include <iostream>
#include <mutex>

namespace ocpp
{
namespace log
{

/** @brief Loggers */
std::map<std::string, std::unique_ptr<LogDatabase>> Logger::m_loggers;
/** @brief Default logger */
LogDatabase* Logger::m_default_logger = nullptr;

/** @brief Constructor with default logger */
Logger::Logger(const char* level_str, unsigned int level, const char* filename, const char* line)
    : m_log_output(), m_log_database(m_default_logger), m_level_str(level_str), m_level(level), m_filename(filename), m_line(line)
{
}

/** @brief Constructor */
Logger::Logger(const char* name, const char* level_str, unsigned int level, const char* filename, const char* line)
    : m_log_output(), m_log_database(nullptr), m_level_str(level_str), m_level(level), m_filename(filename), m_line(line)
{
    auto iter = m_loggers.find(name);
    if (iter != m_loggers.end())
    {
        m_log_database = iter->second.get();
    }
}

/** @brief Destructor */
Logger::~Logger()
{
    static std::mutex mutex;

    std::time_t       now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream file_line;
    file_line << m_filename << ":" << m_line;
    {
        std::lock_guard<std::mutex> lock(mutex);

        std::tm now_tm;
        localtime_r(&now, &now_tm);
        LOG_OUTPUT << m_level_str << " - [" << std::put_time(&now_tm, "%Y-%m-%dT%T") << "] - " << file_line.str() << " - "
                   << m_log_output.str() << std::endl;
    }
    if (m_log_database)
    {
        m_log_database->log(now, m_level, file_line.str(), m_log_output.str());
    }
}

/** @brief Register the default logger */
void Logger::registerDefaultLogger(ocpp::database::Database& database, unsigned int max_entries)
{
    auto iter = m_loggers.find(DEFAULT_LOG_NAME);
    if (iter == m_loggers.end())
    {
        m_default_logger = new LogDatabase(database, DEFAULT_LOG_NAME, max_entries);
        m_loggers.emplace(DEFAULT_LOG_NAME, m_default_logger);
    }
}

/** @brief Unregister the default logger */
void Logger::unregisterDefaultLogger()
{
    auto iter = m_loggers.find(DEFAULT_LOG_NAME);
    if (iter != m_loggers.end())
    {
        m_loggers.erase(iter);
    }
}

/** @brief Register a logger */
void Logger::registerLogger(ocpp::database::Database& database, const std::string& name, unsigned int max_entries)
{
    auto iter = m_loggers.find(name);
    if (iter == m_loggers.end())
    {
        m_loggers.emplace(name, new LogDatabase(database, name, max_entries));
    }
}

} // namespace log
} // namespace ocpp
