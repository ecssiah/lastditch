// ============================================================================
// File:    src/core/log.c
// Layer:   core
// Purpose: Implementation of the session log declared in core/log.h.
// ============================================================================

#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>

// ----------------------------------------------------------------------------
// Constants: log limits
// Purpose:   Fixed buffer sizes used by the log. The log must work when
//            memory is exhausted, so it never allocates per message.
// ----------------------------------------------------------------------------
#define LOG_PATH_CAPACITY 1024      // Longest log file path, with terminator.
#define LOG_MESSAGE_CAPACITY 4096   // Longest formatted message, with terminator.
#define LOG_TIMESTAMP_CAPACITY 32   // Longest formatted timestamp, with terminator.

// ----------------------------------------------------------------------------
// Constants: build configuration name
// Purpose:   Name of the build configuration reported by the opening log
//            message. NDEBUG is the standard macro that disables assert.
// ----------------------------------------------------------------------------
#ifdef NDEBUG
#define LOG_BUILD_CONFIGURATION "RELEASE"   // Asserts are compiled out.
#else
#define LOG_BUILD_CONFIGURATION "DEBUG"     // Asserts are active.
#endif

// ----------------------------------------------------------------------------
// State:   LogState
// Purpose: Everything the log remembers between calls: the open files and
//          the call site recorded by log_begin for the message in flight.
// ----------------------------------------------------------------------------
typedef struct LogState
{
    FILE* file;                     // Log file under the working directory, or NULL.
    FILE* repository_file;          // Log file under the repository root, or NULL.
    LogLevel pending_level;         // Severity recorded by the last log_begin.
    const char* pending_file;       // Source file recorded by the last log_begin.
    int pending_line;               // Source line recorded by the last log_begin.
} LogState;

// ----------------------------------------------------------------------------
// State:   log_state
// Purpose: The one instance of the log state. Static storage zero-initialises
//          it, so logging works (to the console only) before log_init.
// ----------------------------------------------------------------------------
static LogState log_state;

// ----------------------------------------------------------------------------
// Function: log_level_to_string
// Purpose:  Names a severity for the message prefix.
// Params:   level - severity to name
// Returns:  A static upper-case name, "UNKNOWN" for values outside LogLevel.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
static const char*
log_level_to_string(const LogLevel level)
{
    switch (level)
    {
        case LOG_LEVEL_TRACE:  return "TRACE";
        case LOG_LEVEL_INFO:   return "INFO";
        case LOG_LEVEL_WARN:   return "WARN";
        case LOG_LEVEL_ERROR:  return "ERROR";
        case LOG_LEVEL_FATAL:  return "FATAL";
        default:               return "UNKNOWN";
    }
}

// ----------------------------------------------------------------------------
// Function: log_path_exists
// Purpose:  Tests whether anything exists at a file system path.
// Params:   path - path to test; an empty path never exists
// Returns:  true when a file or directory exists at path.
// Reads:    the file system
// Writes:   nothing
// ----------------------------------------------------------------------------
static int
log_path_exists(const char* path)
{
    if (path[0] == '\0')
    {
        return false;
    }

    return SDL_GetPathInfo(path, NULL) ? true : false;
}

// ----------------------------------------------------------------------------
// Function: log_path_is_separator
// Purpose:  Tests whether a character separates path components.
// Params:   character - character to test
// Returns:  true for '/' and '\\'.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
static int
log_path_is_separator(const char character)
{
    return character == '/' || character == '\\';
}

// ----------------------------------------------------------------------------
// Function: log_path_to_parent
// Purpose:  Shortens a directory path, in place, to its parent directory.
// Params:   directory - directory path that ends with a separator
// Returns:  true when the path was shortened; false when it was already a
//           file system root and has been left unchanged.
// Reads:    directory
// Writes:   directory
// ----------------------------------------------------------------------------
static int
log_path_to_parent(char* directory)
{
    size_t length;

    length = strlen(directory);

    // Skip the trailing separator of the directory itself.
    if (length > 0 && log_path_is_separator(directory[length - 1]))
    {
        --length;
    }

    while (length > 0 && !log_path_is_separator(directory[length - 1]))
    {
        --length;
    }

    if (length == 0)
    {
        return false;
    }

    directory[length] = '\0';

    return true;
}

// ----------------------------------------------------------------------------
// Function: log_find_repository_root
// Purpose:  Searches the working directory and its ancestors for the root of
//           a repository checkout, recognised by an entry named ".git".
// Params:   root          - receives the root directory, ending with a
//                           separator; set to "" when none is found
//           root_capacity - size of the root buffer in bytes
//           is_working_directory - receives true when the root found is the
//                           working directory itself
// Returns:  nothing
// Reads:    the file system
// Writes:   root, is_working_directory
// ----------------------------------------------------------------------------
static void
log_find_repository_root(char* root, const size_t root_capacity, int* is_working_directory)
{
    char candidate[LOG_PATH_CAPACITY];
    char marker[LOG_PATH_CAPACITY];
    char* working_directory;
    int is_first;

    root[0] = '\0';
    *is_working_directory = false;

    working_directory = SDL_GetCurrentDirectory();

    if (working_directory == NULL)
    {
        return;
    }

    if (strlen(working_directory) >= sizeof(candidate) - 8)
    {
        SDL_free(working_directory);
        return;
    }

    strcpy(candidate, working_directory);
    SDL_free(working_directory);

    is_first = true;

    for (;;)
    {
        strcpy(marker, candidate);
        strcat(marker, ".git");

        if (log_path_exists(marker))
        {
            if (strlen(candidate) < root_capacity)
            {
                strcpy(root, candidate);
                *is_working_directory = is_first;
            }

            return;
        }

        if (!log_path_to_parent(candidate))
        {
            return;
        }

        is_first = false;
    }
}

// ----------------------------------------------------------------------------
// Function: log_format_path
// Purpose:  Builds "<directory><prefix><suffix>.log" in a path buffer.
// Params:   path      - receives the path; "" when it would not fit
//           directory - directory ending with a separator; "" yields ""
//           prefix    - file name without extension
//           sequence  - 0 for no suffix, otherwise appended as "_<sequence>"
// Returns:  nothing
// Reads:    arguments only
// Writes:   path
// Notes:    path must hold LOG_PATH_CAPACITY bytes.
// ----------------------------------------------------------------------------
static void
log_format_path(char* path, const char* directory, const char* prefix, const u32 sequence)
{
    int length;

    if (directory[0] == '\0')
    {
        path[0] = '\0';
        return;
    }

    if (sequence == 0)
    {
        length = SDL_snprintf(path, LOG_PATH_CAPACITY, "%s%s.log", directory, prefix);
    }
    else
    {
        length = SDL_snprintf(path, LOG_PATH_CAPACITY, "%s%s_%u.log", directory, prefix, (unsigned int)sequence);
    }

    if (length < 0 || length >= LOG_PATH_CAPACITY)
    {
        path[0] = '\0';
    }
}

// Documented in src/core/log.h.
void
log_init(void)
{
    static const char log_directory[] = "logs/";

    char session_timestamp[LOG_TIMESTAMP_CAPACITY];
    char filename_prefix[LOG_TIMESTAMP_CAPACITY + 8];
    char path[LOG_PATH_CAPACITY];
    char repository_root[LOG_PATH_CAPACITY];
    char repository_log_directory[LOG_PATH_CAPACITY];
    char repository_path[LOG_PATH_CAPACITY];
    int root_is_working_directory;
    time_t now;
    const struct tm* time_info;
    u32 sequence;

    SDL_CreateDirectory(log_directory);

    now = time(NULL);
    time_info = localtime(&now);

    session_timestamp[0] = '\0';

    if (time_info != NULL)
    {
        strftime(session_timestamp, sizeof(session_timestamp), "%Y_%m_%d_%H_%M_%S", time_info);
    }

    strcpy(filename_prefix, "engine_");
    strcat(filename_prefix, session_timestamp);

    repository_log_directory[0] = '\0';

    log_find_repository_root(repository_root, sizeof(repository_root) - 8, &root_is_working_directory);

    if (repository_root[0] != '\0')
    {
        strcpy(repository_log_directory, repository_root);
        strcat(repository_log_directory, "logs/");

        SDL_CreateDirectory(repository_log_directory);

        if (root_is_working_directory)
        {
            // Both locations are the same directory; one file is enough.
            repository_log_directory[0] = '\0';
        }
    }

    log_format_path(path, log_directory, filename_prefix, 0);
    log_format_path(repository_path, repository_log_directory, filename_prefix, 0);

    for (sequence = 2; log_path_exists(path) || log_path_exists(repository_path); ++sequence)
    {
        log_format_path(path, log_directory, filename_prefix, sequence);
        log_format_path(repository_path, repository_log_directory, filename_prefix, sequence);
    }

    log_state.file = path[0] != '\0' ? fopen(path, "w") : NULL;
    log_state.repository_file = repository_path[0] != '\0' ? fopen(repository_path, "w") : NULL;

    LOG_INFO("\n\nLOG INIT (%s)\n", LOG_BUILD_CONFIGURATION);
}

// Documented in src/core/log.h.
LogWriter
log_begin(const LogLevel level, const char* file, const int line)
{
    log_state.pending_level = level;
    log_state.pending_file = file;
    log_state.pending_line = line;

    return log_write;
}

// Documented in src/core/log.h.
void
log_write(const char* format, ...)
{
    char timestamp[LOG_TIMESTAMP_CAPACITY];
    char message[LOG_MESSAGE_CAPACITY];
    const LogLevel level = log_state.pending_level;
    const char* level_string;
    const char* filename;
    const char* last_slash;
    const struct tm* time_info;
    FILE* stream_array[3];
    time_t now;
    va_list arguments;
    s32 stream_index;

    now = time(NULL);
    time_info = localtime(&now);

    timestamp[0] = '\0';

    if (time_info != NULL)
    {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);
    }

    filename = log_state.pending_file != NULL ? log_state.pending_file : "";
    last_slash = strrchr(filename, '/');

    if (last_slash != NULL)
    {
        filename = last_slash + 1;
    }

    // ANSI C89 has no va_copy, so the message is formatted once into a buffer
    // and the buffer is then written to every destination.
    va_start(arguments, format);
    SDL_vsnprintf(message, sizeof(message), format, arguments);
    va_end(arguments);

    level_string = log_level_to_string(level);

    stream_array[0] = level >= LOG_LEVEL_WARN ? stderr : stdout;
    stream_array[1] = log_state.file;
    stream_array[2] = log_state.repository_file;

    for (stream_index = 0; stream_index < ARRAY_COUNT(stream_array); ++stream_index)
    {
        FILE* stream = stream_array[stream_index];

        if (stream == NULL)
        {
            continue;
        }

        fprintf(stream, "[%s] [%s] (%s:%d) ", timestamp, level_string, filename, log_state.pending_line);
        fputs(message, stream);
        fputs("\n", stream);

        if (stream_index > 0 || level == LOG_LEVEL_FATAL)
        {
            fflush(stream);
        }
    }

    if (level == LOG_LEVEL_FATAL)
    {
        exit(EXIT_FAILURE);
    }
}

// Documented in src/core/log.h.
void
log_quit(void)
{
    LOG_INFO("\n\nLOG CLOSE\n");

    if (log_state.file != NULL)
    {
        fclose(log_state.file);
        log_state.file = NULL;
    }

    if (log_state.repository_file != NULL)
    {
        fclose(log_state.repository_file);
        log_state.repository_file = NULL;
    }
}
