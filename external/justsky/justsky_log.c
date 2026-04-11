#include "justsky_log.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

static FILE* JUSTSKY_LOG_FILE;
static char JUSTSKY_LOG_BASE_PATH[256];
static char JUSTSKY_CURRENT_DAY_STRING[11];

static const char* LOG_LEVEL_TO_STRING[LOG_LEVEL_COUNT] =
{
    "TRACE",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};


void log_init()
{
    const char* directory_name = "log/";

    strncpy(JUSTSKY_LOG_BASE_PATH, directory_name, sizeof(JUSTSKY_LOG_BASE_PATH) - 1);

    if (!JUSTSKY_LOG_FILE)
    {
        JUSTSKY_LOG_FILE = stderr;
    }

    LOG_INFO("\n\nLOG INIT\n");
}

void log_message(ELogLevel log_level, const char* file, int line, const char* fmt, ...)
{
    if (!JUSTSKY_LOG_FILE)
    {
        JUSTSKY_LOG_FILE = stderr;
    }

    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);

    char file_timestamp[11];
    strftime(file_timestamp, sizeof(file_timestamp), "%Y_%m_%d", &tm_info);

    if (JUSTSKY_LOG_BASE_PATH[0] != '\0' && strcmp(file_timestamp, JUSTSKY_CURRENT_DAY_STRING) != 0)
    {
        if (JUSTSKY_LOG_FILE && JUSTSKY_LOG_FILE != stderr)
        {
            fclose(JUSTSKY_LOG_FILE);
        }

        strncpy(JUSTSKY_CURRENT_DAY_STRING, file_timestamp, sizeof(JUSTSKY_CURRENT_DAY_STRING) - 1);

        char path[512];
        snprintf(path, sizeof(path), "%sengine_%s.log", JUSTSKY_LOG_BASE_PATH, file_timestamp);

        JUSTSKY_LOG_FILE = fopen(path, "a");
        if (!JUSTSKY_LOG_FILE)
        {
            JUSTSKY_LOG_FILE = stderr;
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
        LOG_LEVEL_TO_STRING[log_level],
        filename,
        line
    );

    if (JUSTSKY_LOG_FILE && JUSTSKY_LOG_FILE != stderr) 
    {
        fprintf(
            JUSTSKY_LOG_FILE, 
            "[%s] [%s] (%s:%d) ",
            timestamp,
            LOG_LEVEL_TO_STRING[log_level],
            filename,
            line
        );
    }

    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);

    vfprintf(stderr, fmt, args);

    if (JUSTSKY_LOG_FILE && JUSTSKY_LOG_FILE != stderr)
    {
        vfprintf(JUSTSKY_LOG_FILE, fmt, args_copy);
    }

    va_end(args_copy);
    va_end(args);

    fprintf(stderr, "\n");

    if (JUSTSKY_LOG_FILE && JUSTSKY_LOG_FILE != stderr) 
    {
        fprintf(JUSTSKY_LOG_FILE, "\n");
        fflush(JUSTSKY_LOG_FILE);
    }

    if (log_level == LOG_LEVEL_FATAL)
    {
        fflush(stderr);

        if (JUSTSKY_LOG_FILE && JUSTSKY_LOG_FILE != stderr) 
        {
            fflush(JUSTSKY_LOG_FILE);
        }

        exit(EXIT_FAILURE);
    }
}

void log_close()
{
    LOG_INFO("\n\nLOG CLOSE\n");

    if (JUSTSKY_LOG_FILE && JUSTSKY_LOG_FILE != stderr)
    {
        fclose(JUSTSKY_LOG_FILE);
    }
}
