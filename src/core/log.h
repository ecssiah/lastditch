#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include "core/types.h"

#define LOG_TRACE(fmt, ...) Log::message(LogLevel::Trace, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  Log::message(LogLevel::Info,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  Log::message(LogLevel::Warn,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Log::message(LogLevel::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Log::message(LogLevel::Fatal, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

enum class LogLevel : u8
{
    Trace,
    Info,
    Warn,
    Error,
    Fatal,
};

class Log
{
    inline static std::ofstream            ld_log_file {};
    inline static std::filesystem::path    ld_log_directory {};
    inline static std::string              ld_current_day_string {};

    static constexpr const char* log_level_to_string(LogLevel level);

public:
    static void init();
    static void quit();

    static void message(LogLevel log_level, const char* log_file, int line, const char* fmt, ...);
};

