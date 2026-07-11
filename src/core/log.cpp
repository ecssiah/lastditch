#include "log.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <filesystem>
#include <string>

using namespace std;

constexpr const char*
Log::log_level_to_string(const LogLevel level)
{
    switch (level)
    {
        case LogLevel::Trace:  return "TRACE";
        case LogLevel::Info:   return "INFO";
        case LogLevel::Warn:   return "WARN";
        case LogLevel::Error:  return "ERROR";
        case LogLevel::Fatal:  return "FATAL";
    }

    return "UNKNOWN";
}

void 
Log::init()
{
    ld_log_directory = "logs";

    filesystem::create_directories(ld_log_directory);

    LOG_INFO("\n\nLOG INIT\n");
}

void 
Log::message(LogLevel log_level, const char* file, int line, const char* fmt, ...)
{
    const time_t now { time(nullptr) };
    tm tm_info {};
    localtime_r(&now, &tm_info);

    char file_timestamp_buffer[11] {};
    strftime(file_timestamp_buffer, sizeof(file_timestamp_buffer), "%Y_%m_%d", &tm_info);

    const std::string file_timestamp { file_timestamp_buffer };

    if (file_timestamp != ld_current_day_string)
    {
        if (ld_log_file.is_open())
        {
            ld_log_file.close();
        }

        ld_current_day_string = file_timestamp;

        const auto path { ld_log_directory / ("engine_" + file_timestamp + ".log") };

        ld_log_file.open(path, std::ios::app);
    }

    const char* filename { file };
    const char* last_slash = { strrchr(file, '/') };

    if (last_slash)
    {
        filename = last_slash + 1;
    }

    char timestamp[32] {};

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_info);

    FILE* console_stream { log_level >= LogLevel::Warn ? stderr : stdout };

    fprintf(
        console_stream,
        "[%s] [%s] (%s:%d) ",
        timestamp,
        log_level_to_string(log_level),
        filename,
        line
    );
    
    if (ld_log_file.is_open())
    {
        ld_log_file
            << "[" << timestamp << "] "
            << "[" << log_level_to_string(log_level) << "] "
            << "(" << filename << ":" << line << ") ";
    }

    va_list args {};
    va_start(args, fmt);

    va_list args_copy {};
    va_copy(args_copy, args);

    vfprintf(console_stream, fmt, args);
    if (ld_log_file.is_open())
    {
        char file_message_buffer[4096] {};

        vsnprintf(file_message_buffer, sizeof(file_message_buffer), fmt, args_copy);

        ld_log_file << file_message_buffer;
    }

    va_end(args_copy);
    va_end(args);

    fprintf(console_stream, "\n");

    if (ld_log_file.is_open())
    {
        ld_log_file << "\n";
        ld_log_file.flush();
    }

    if (log_level == LogLevel::Fatal)
    {
        fflush(console_stream);

        if (ld_log_file.is_open())
        {
            ld_log_file.flush();
        }

        exit(EXIT_FAILURE);
    }
}

void 
Log::quit()
{
    LOG_INFO("\n\nLOG CLOSE\n");

    if (ld_log_file.is_open())
    {
        ld_log_file.close();
    }
}
