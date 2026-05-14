#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

static FILE* ld_log_file;
static char ld_log_base_path[256];
static char ld_current_day_string[11];

constexpr const char*
log_level_to_string(const LogLevel level)
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
log_init()
{
    auto directory_name = "log/";

    strncpy(ld_log_base_path, directory_name, sizeof(ld_log_base_path) - 1);

    if (!ld_log_file)
    {
        ld_log_file = stderr;
    }

    LOG_INFO("\n\nLOG INIT\n");
}

void 
log_message(LogLevel log_level, const char* file, int line, const char* fmt, ...)
{
    if (!ld_log_file)
    {
        ld_log_file = stderr;
    }

    const time_t now = time(NULL);
    tm tm_info;
    localtime_r(&now, &tm_info);

    char file_timestamp[11];
    strftime(file_timestamp, sizeof(file_timestamp), "%Y_%m_%d", &tm_info);

    if (ld_log_base_path[0] != '\0' && strcmp(file_timestamp, ld_current_day_string) != 0)
    {
        if (ld_log_file && ld_log_file != stderr)
        {
            fclose(ld_log_file);
        }

        strncpy(ld_current_day_string, file_timestamp, sizeof(ld_current_day_string) - 1);

        char path[512];
        snprintf(path, sizeof(path), "%sengine_%s.log", ld_log_base_path, file_timestamp);

        ld_log_file = fopen(path, "a");
        if (!ld_log_file)
        {
            ld_log_file = stderr;
        }
    }

    const char* filename = file;
    const char* last_slash = strrchr(file, '/');

    if (last_slash)
    {
        filename = last_slash + 1;
    }

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_info);

    fprintf(
        stderr,
        "[%s] [%s] (%s:%d) ",
        timestamp,
        log_level_to_string(log_level),
        filename,
        line
    );

    if (ld_log_file && ld_log_file != stderr)
    {
        fprintf(
            ld_log_file,
            "[%s] [%s] (%s:%d) ",
            timestamp,
            log_level_to_string(log_level),
            filename,
            line
        );
    }

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);

    vfprintf(stderr, fmt, args);

    if (ld_log_file && ld_log_file != stderr)
    {
        vfprintf(ld_log_file, fmt, args_copy);
    }

    va_end(args_copy);
    va_end(args);

    fprintf(stderr, "\n");

    if (ld_log_file && ld_log_file != stderr)
    {
        fprintf(ld_log_file, "\n");
        fflush(ld_log_file);
    }

    if (log_level == LogLevel::Fatal)
    {
        fflush(stderr);

        if (ld_log_file && ld_log_file != stderr)
        {
            fflush(ld_log_file);
        }

        exit(EXIT_FAILURE);
    }
}

void 
log_close()
{
    LOG_INFO("\n\nLOG CLOSE\n");

    if (ld_log_file && ld_log_file != stderr)
    {
        fclose(ld_log_file);
    }
}
