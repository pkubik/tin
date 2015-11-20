/*
 * PDI 2015
 *
 * Pawel Kubik
 */

#pragma once

#include <string>
#include <ostream>
#include <iostream>

enum LogLevel
{
    TRACE_LEVEL,
    DEBUG_LEVEL,
    INFO_LEVEL,
    WARN_LEVEL,
    ERROR_LEVEL,
    HELP_LEVEL
};

constexpr char* LOG_LEVEL_STRINGS[] =
{
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "HELP"
};

constexpr const char* toString(LogLevel severity)
{
    return LOG_LEVEL_STRINGS[severity];
}

class Logger
{
    std::ostream* stream = &std::cerr;
#ifdef NDEBUG
    LogLevel minLevel = INFO_LEVEL;
#else
    LogLevel minLevel = TRACE_LEVEL;
#endif

    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator = (const Logger&) = delete;

    static Logger& logger()
    {
        static Logger logger;
        return logger;
    }

public:
    static void setLevel(LogLevel level)
    {
        logger().minLevel = level;
    }

    static void setBackend(std::ostream& stream)
    {
        logger().stream = &stream;
    }

    static void log(LogLevel level,
                    const std::string& message,
                    const char*, unsigned line,
                    const char* function)
    {
        if (logger().minLevel <= level)
        {
            *logger().stream << "[" << toString(level) << "]\t"
                << function << ':' << line
                << "\t-:  " << message << std::endl;
        }
    }
};

#define LOG(LEVEL, MESSAGE) do { Logger::log(LEVEL, MESSAGE, __FILE__, __LINE__, __func__); } while (0,0)

#define LOGE(MESSAGE) LOG(ERROR_LEVEL, MESSAGE)
#define LOGW(MESSAGE) LOG(WARN_LEVEL, MESSAGE)
#define LOGI(MESSAGE) LOG(INFO_LEVEL, MESSAGE)

#ifndef NDEBUG

#define LOGD(MESSAGE) LOG(DEBUG_LEVEL, MESSAGE)
#define LOGH(MESSAGE) LOG(HELP_LEVEL, MESSAGE)
#define LOGT(MESSAGE) LOG(TRACE_LEVEL, MESSAGE)

#else

#define LOGD(MESSAGE) do {} while (0)
#define LOGH(MESSAGE) do {} while (0)
#define LOGT(MESSAGE) do {} while (0)

#endif
