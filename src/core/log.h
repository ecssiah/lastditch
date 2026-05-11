#pragma once

#define LOG_TRACE(fmt, ...) log_message(ELogLevel::trace, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  log_message(ELogLevel::info,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  log_message(ELogLevel::warn,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_message(ELogLevel::error, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) log_message(ELogLevel::fatal, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

enum class ELogLevel
{
    trace,
    info,
    warn,
    error,
    fatal,
};

void log_message(ELogLevel log_level, const char* log_file, int line, const char* fmt, ...);

void log_init();
void log_close();
