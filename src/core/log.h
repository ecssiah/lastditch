#pragma once

#include "core/types.h"

#define LOG_TRACE(fmt, ...) log_message(LogLevel::trace, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_message(LogLevel::info,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_message(LogLevel::warn,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LogLevel::error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) log_message(LogLevel::fatal, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

enum class LogLevel : u8
{
    trace,
    info,
    warn,
    error,
    fatal,
};

void log_message(LogLevel log_level, const char* log_file, int line, const char* fmt, ...);

void log_init();
void log_close();
