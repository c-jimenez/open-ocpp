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

#ifndef OPENOCPP_LOGGER_H
#define OPENOCPP_LOGGER_H

#include "FilenameMacro.h"

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace ocpp
{
namespace database
{
class Database;
}
namespace log
{

class LogDatabase;

#ifndef LOG_OUTPUT
/** @brief Default log output goes to std::cout
 *         Override this define to redirect to another output
 *         Any object implementing the following operator will work :
 *
 *         CustomObject& CustomObject::operator << (const std::string& str);
 */
#define LOG_OUTPUT std::cout
#endif

/*
    Log level is defined as follow:
    - LOG_DEBUG = 0
    - LOG_COM = 1
    - LOG_INFO = 2
    - LOG_WARNING = 3
    - LOG_ERROR = 4
    - NO_LOGS = 5 or greater
*/
#ifndef LOG_LEVEL
/** @brief Default log level if nothing is specified => INFO */
#define LOG_LEVEL 2
#endif // LOG_LEVEL

#ifdef EXTERNAL_LOGGER
/** @brief Log level is set to 0 to generate all the logs
 *         The leve filtering will be done by the external logger
*/
#undef LOG_LEVEL
#define LOG_LEVEL 0

// Use the external logger
#define OPENOCPP_LOGGER ocpp::log::ExtLogger

#else

// Use the standard logger
#define OPENOCPP_LOGGER ocpp::log::Logger

#endif // EXTERNAL_LOGGER

/** @brief Default log name */
#define DEFAULT_LOG_NAME "Logs"

/** @brief Macro to stringify an expression */
#define LINE_STRINGIFY(x) #x
/** @brief Macro to stringify the current code line */
#define LINE_TOSTRING(x) LINE_STRINGIFY(x)

// Log macros
#if (LOG_LEVEL == 0)
#define LOG_DEBUG OPENOCPP_LOGGER("\x1b[32m[ DEBUG ]\x1b[0m", 0, __FILENAME__, LINE_TOSTRING(__LINE__))
#define LOG_DEBUG2(logger_name) OPENOCPP_LOGGER(logger_name, "\x1b[32m[ DEBUG ]\x1b[0m", 0, __FILENAME__, LINE_TOSTRING(__LINE__))
#else
#define LOG_DEBUG ocpp::log::NullLogger()
#define LOG_DEBUG2(logger_name) ocpp::log::NullLogger()
#endif
#if (LOG_LEVEL <= 1)
#define LOG_COM OPENOCPP_LOGGER("\x1b[34m[  COM  ]\x1b[0m", 1, __FILENAME__, LINE_TOSTRING(__LINE__))
#define LOG_COM2(logger_name) OPENOCPP_LOGGER(logger_name, "\x1b[34m[  COM  ]\x1b[0m", 1, __FILENAME__, LINE_TOSTRING(__LINE__))
#else
#define LOG_COM ocpp::log::NullLogger()
#define LOG_COM2(logger_name) ocpp::log::NullLogger()
#endif
#if (LOG_LEVEL <= 2)
#define LOG_INFO OPENOCPP_LOGGER("\x1b[30m[ INFO  ]\x1b[0m", 2, __FILENAME__, LINE_TOSTRING(__LINE__))
#define LOG_INFO2(logger_name) OPENOCPP_LOGGER(logger_name, "\x1b[30m[ INFO  ]\x1b[0m", 2, __FILENAME__, LINE_TOSTRING(__LINE__))
#else
#define LOG_INFO ocpp::log::NullLogger()
#define LOG_INFO2(logger_name) ocpp::log::NullLogger()
#endif
#if (LOG_LEVEL <= 3)
#define LOG_WARNING OPENOCPP_LOGGER("\x1b[33m[WARNING]\x1b[0m", 3, __FILENAME__, LINE_TOSTRING(__LINE__))
#define LOG_WARNING2(logger_name) OPENOCPP_LOGGER(logger_name, "\x1b[33m[WARNING]\x1b[0m", 3, __FILENAME__, LINE_TOSTRING(__LINE__))
#else
#define LOG_WARNING ocpp::log::NullLogger()
#define LOG_WARNING2(logger_name) ocpp::log::NullLogger()
#endif
#if (LOG_LEVEL <= 4)
#define LOG_ERROR OPENOCPP_LOGGER("\x1b[31m[ ERROR ]\x1b[0m", 4, __FILENAME__, LINE_TOSTRING(__LINE__))
#define LOG_ERROR2(logger_name) OPENOCPP_LOGGER(logger_name, "\x1b[31m[ ERROR ]\x1b[0m", 4, __FILENAME__, LINE_TOSTRING(__LINE__))
#else
#define LOG_ERROR ocpp::log::NullLogger()
#define LOG_ERROR2(logger_name) ocpp::log::NullLogger()
#endif

/** @brief Logger */
class Logger
{
  public:
    /** @brief Constructor with default logger */
    Logger(const char* level_str, unsigned int level, const char* filename, const char* line);
    /** @brief Constructor */
    Logger(const char* name, const char* level_str, unsigned int level, const char* filename, const char* line);

    /** @brief Destructor */
    virtual ~Logger();

    /**
     * @brief Generic log operator
     * @param input Data to log
     * @return Logger's instance
     */
    template <typename T>
    Logger& operator<<(const T& input)
    {
        m_log_output << input;
        return (*this);
    }

    /**
     * @brief Log operator for boolean values
     * @param input Data to log
     * @return Logger's instance
     */
    Logger& operator<<(bool input)
    {
        static const char* BOOL_STRING[] = {"false", "true"};
        m_log_output << BOOL_STRING[static_cast<int>(input)];
        return (*this);
    }

    /** @brief Register the default logger */
    static void registerDefaultLogger(ocpp::database::Database& database, unsigned int max_entries);
    /** @brief Unregister the default logger */
    static void unregisterDefaultLogger();
    /** @brief Register a logger */
    static void registerLogger(ocpp::database::Database& database, const std::string& name, unsigned int max_entries);

  private:
    /** @brief Log output */
    std::stringstream m_log_output;
    /** @brief Log database */
    LogDatabase* m_log_database;
    /** @brief Log level */
    const char* m_level_str;
    /** @brief Log level */
    unsigned int m_level;
    /** @brief File name */
    const char* m_filename;
    /** @brief Code line */
    const char* m_line;

    /** @brief Loggers */
    static std::map<std::string, std::unique_ptr<LogDatabase>> m_loggers;
    /** @brief Default logger */
    static LogDatabase* m_default_logger;
};

/** @brief Null logger */
class NullLogger
{
  public:
    /**
     * @brief Generic log operator
     * @param input Data to log
     * @return Logger's instance
     */
    template <typename T>
    constexpr NullLogger& operator<<(const T& input)
    {
        // Does nothing
        (void)input;
        return (*this);
    }
};

/** @brief External logger */
class ExtLogger
{
  public:
    /** @brief Constructor */
    ExtLogger(const char* level_str, unsigned int level, const char* filename, const char* line);
    /** @brief Constructor */
    ExtLogger(const char* name, const char* level_str, unsigned int level, const char* filename, const char* line);

    /** @brief Destructor */
    virtual ~ExtLogger();

    /**
     * @brief Generic log operator
     * @param input Data to log
     * @return Logger's instance
     */
    template <typename T>
    ExtLogger& operator<<(const T& input)
    {
        m_log_output << input;
        return (*this);
    }

    /**
     * @brief Log operator for boolean values
     * @param input Data to log
     * @return Logger's instance
     */
    ExtLogger& operator<<(bool input)
    {
        static const char* BOOL_STRING[] = {"false", "true"};
        m_log_output << BOOL_STRING[static_cast<int>(input)];
        return (*this);
    }

    /** @brief Register an external logging function */
    static void registerLogFunction(std::function<void(unsigned int, const std::string&)> log_function);

  private:
    /** @brief Log output */
    std::stringstream m_log_output;
    /** @brief Log level */
    unsigned int m_level;

    /** @brief External logging function */
    static std::function<void(unsigned int, const std::string&)> m_log_function;
};

} // namespace log
} // namespace ocpp

#endif // OPENOCPP_LOGDATABASE_H
