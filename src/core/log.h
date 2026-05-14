#pragma once

#include "core/types.h"

#define LOG_TRACE(fmt, ...) log_message(LogLevel::Trace, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_message(LogLevel::Info,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_message(LogLevel::Warn,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(LogLevel::Error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) log_message(LogLevel::Fatal, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

enum class LogLevel : u8
{
    Trace,
    Info,
    Warn,
    Error,
    Fatal,
};

void log_message(LogLevel log_level, const char* log_file, int line, const char* fmt, ...);

void log_init();
void log_close();
