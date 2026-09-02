#include "log.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <string>

using namespace std;

namespace
{
#ifdef NDEBUG
constexpr const char* BUILD_CONFIGURATION { "RELEASE" };
#else
constexpr const char* BUILD_CONFIGURATION { "DEBUG" };
#endif

filesystem::path
find_repository_root()
{
    auto candidate { filesystem::current_path() };

    while (true)
    {
        if (filesystem::exists(candidate / ".git"))
        {
            return candidate;
        }

        const auto parent { candidate.parent_path() };
        if (parent == candidate)
        {
            return {};
        }

        candidate = parent;
    }
}
}

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
        default:               return "UNKNOWN";
    }
}

void
Log::init()
{
    ld_log_directory = "logs";

    filesystem::create_directories(ld_log_directory);

    const time_t now { time(nullptr) };
    tm tm_info {};

#ifdef _WIN32
    localtime_s(&tm_info, &now);
#else
    localtime_r(&now, &tm_info);
#endif

    char session_timestamp[20] {};
    strftime(session_timestamp, sizeof(session_timestamp), "%Y_%m_%d_%H_%M_%S", &tm_info);

    const string filename_prefix { "engine_" + string { session_timestamp } };

    auto path { ld_log_directory / (filename_prefix + ".log") };
    filesystem::path repository_log_directory {};
    filesystem::path repository_path {};

    const auto repository_root { find_repository_root() };
    if (!repository_root.empty())
    {
        repository_log_directory = repository_root / "logs";
        filesystem::create_directories(repository_log_directory);

        if (filesystem::absolute(ld_log_directory).lexically_normal() == repository_log_directory.lexically_normal())
        {
            repository_log_directory.clear();
        }
        else
        {
            repository_path = repository_log_directory / (filename_prefix + ".log");
        }
    }

    for (
        u32 sequence { 2 };
        filesystem::exists(path) || (!repository_path.empty() && filesystem::exists(repository_path));
        ++sequence
    )
    {
        const auto filename { filename_prefix + "_" + to_string(sequence) + ".log" };
        path = ld_log_directory / filename;

        if (!repository_log_directory.empty())
        {
            repository_path = repository_log_directory / filename;
        }
    }

    ld_log_file.open(path, ios::out);

    if (!repository_path.empty())
    {
        ld_repository_log_file.open(repository_path, ios::out);
    }

    LOG_INFO("\n\nLOG INIT (%s)\n", BUILD_CONFIGURATION);
}

void
Log::message(LogLevel log_level, const char* file, const int line, const char* fmt, ...)
{
    const time_t now { time(nullptr) };
    tm tm_info {};

#ifdef _WIN32
    localtime_s(&tm_info, &now);
#else
    localtime_r(&now, &tm_info);
#endif

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

    if (ld_repository_log_file.is_open())
    {
        ld_repository_log_file
            << "[" << timestamp << "] "
            << "[" << log_level_to_string(log_level) << "] "
            << "(" << filename << ":" << line << ") ";
    }

    va_list args {};
    va_start(args, fmt);

    va_list args_copy {};
    va_copy(args_copy, args);

    vfprintf(console_stream, fmt, args);
    if (ld_log_file.is_open() || ld_repository_log_file.is_open())
    {
        char file_message_buffer[4096] {};

        vsnprintf(file_message_buffer, sizeof(file_message_buffer), fmt, args_copy);

        if (ld_log_file.is_open())
        {
            ld_log_file << file_message_buffer;
        }

        if (ld_repository_log_file.is_open())
        {
            ld_repository_log_file << file_message_buffer;
        }
    }

    va_end(args_copy);
    va_end(args);

    fprintf(console_stream, "\n");

    if (ld_log_file.is_open())
    {
        ld_log_file << "\n";
        ld_log_file.flush();
    }

    if (ld_repository_log_file.is_open())
    {
        ld_repository_log_file << "\n";
        ld_repository_log_file.flush();
    }

    if (log_level == LogLevel::Fatal)
    {
        fflush(console_stream);

        if (ld_log_file.is_open())
        {
            ld_log_file.flush();
        }

        if (ld_repository_log_file.is_open())
        {
            ld_repository_log_file.flush();
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

    if (ld_repository_log_file.is_open())
    {
        ld_repository_log_file.close();
    }
}
