#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

static FILE* LD_LOG_FILE;
static char LD_LOG_BASE_PATH[256];
static char LD_CURRENT_DAY_STRING[11];

static const char* LOG_LEVEL_TO_STRING[5] =
{
    "TRACE",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

void 
log_init()
{
    auto directory_name = "log/";

    strncpy(LD_LOG_BASE_PATH, directory_name, sizeof(LD_LOG_BASE_PATH) - 1);

    if (!LD_LOG_FILE)
    {
        LD_LOG_FILE = stderr;
    }

    LOG_INFO("\n\nLOG INIT\n");
}

void 
log_message(LogLevel log_level, const char* file, int line, const char* fmt, ...)
{
    if (!LD_LOG_FILE)
    {
        LD_LOG_FILE = stderr;
    }

    const time_t now = time(NULL);
    tm tm_info;
    localtime_r(&now, &tm_info);

    char file_timestamp[11];
    strftime(file_timestamp, sizeof(file_timestamp), "%Y_%m_%d", &tm_info);

    if (LD_LOG_BASE_PATH[0] != '\0' && strcmp(file_timestamp, LD_CURRENT_DAY_STRING) != 0)
    {
        if (LD_LOG_FILE && LD_LOG_FILE != stderr)
        {
            fclose(LD_LOG_FILE);
        }

        strncpy(LD_CURRENT_DAY_STRING, file_timestamp, sizeof(LD_CURRENT_DAY_STRING) - 1);

        char path[512];
        snprintf(path, sizeof(path), "%sengine_%s.log", LD_LOG_BASE_PATH, file_timestamp);

        LD_LOG_FILE = fopen(path, "a");
        if (!LD_LOG_FILE)
        {
            LD_LOG_FILE = stderr;
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
        LOG_LEVEL_TO_STRING[static_cast<size_t>(log_level)],
        filename,
        line
    );

    if (LD_LOG_FILE && LD_LOG_FILE != stderr)
    {
        fprintf(
            LD_LOG_FILE,
            "[%s] [%s] (%s:%d) ",
            timestamp,
            LOG_LEVEL_TO_STRING[static_cast<size_t>(log_level)],
            filename,
            line
        );
    }

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);

    vfprintf(stderr, fmt, args);

    if (LD_LOG_FILE && LD_LOG_FILE != stderr)
    {
        vfprintf(LD_LOG_FILE, fmt, args_copy);
    }

    va_end(args_copy);
    va_end(args);

    fprintf(stderr, "\n");

    if (LD_LOG_FILE && LD_LOG_FILE != stderr)
    {
        fprintf(LD_LOG_FILE, "\n");
        fflush(LD_LOG_FILE);
    }

    if (log_level == LogLevel::fatal)
    {
        fflush(stderr);

        if (LD_LOG_FILE && LD_LOG_FILE != stderr)
        {
            fflush(LD_LOG_FILE);
        }

        exit(EXIT_FAILURE);
    }
}

void 
log_close()
{
    LOG_INFO("\n\nLOG CLOSE\n");

    if (LD_LOG_FILE && LD_LOG_FILE != stderr)
    {
        fclose(LD_LOG_FILE);
    }
}
