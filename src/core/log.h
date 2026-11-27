// ============================================================================
// File:    src/core/log.h
// Layer:   core
// Purpose: Session logging to the console and to timestamped log files. One
//          log file is written to "logs" under the working directory and,
//          when the program runs from a different directory inside a
//          repository checkout, a second copy is written to the "logs"
//          directory at the repository root.
// ============================================================================

#ifndef CORE_LOG_H
#define CORE_LOG_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Enum:    LogLevel
// Purpose: Severity of a log message, in increasing order. Messages at
//          LOG_LEVEL_WARN and above go to stderr, the rest to stdout.
// ----------------------------------------------------------------------------
typedef enum LogLevel
{
    // Fine-grained diagnostic output.
    LOG_LEVEL_TRACE,
    // Normal progress messages.
    LOG_LEVEL_INFO,
    // Something unexpected but recoverable.
    LOG_LEVEL_WARN,
    // An operation failed.
    LOG_LEVEL_ERROR,
    // Unrecoverable; the process exits after logging.
    LOG_LEVEL_FATAL
} LogLevel;

// ----------------------------------------------------------------------------
// Type:    LogWriter
// Purpose: Signature of the printf-style function that log_begin returns.
//          It exists so that the LOG_* macros can accept a variable number
//          of arguments without variadic macros, which ANSI C89 lacks.
// ----------------------------------------------------------------------------
typedef void (*LogWriter)(const char *format, ...);

// ----------------------------------------------------------------------------
// Macro:   LOG_TRACE, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
// Purpose: Write one formatted message at the named severity, tagged with
//          the source file and line of the call. Used exactly like printf:
//          LOG_INFO("Loaded %d rows", row_count);
//          Each macro expands to a call of log_begin, which records the call
//          site and returns log_write; the argument list written after the
//          macro is then applied to that function.
// ----------------------------------------------------------------------------
#define LOG_TRACE (log_begin(LOG_LEVEL_TRACE, __FILE__, __LINE__))
#define LOG_INFO  (log_begin(LOG_LEVEL_INFO,  __FILE__, __LINE__))
#define LOG_WARN  (log_begin(LOG_LEVEL_WARN,  __FILE__, __LINE__))
#define LOG_ERROR (log_begin(LOG_LEVEL_ERROR, __FILE__, __LINE__))
#define LOG_FATAL (log_begin(LOG_LEVEL_FATAL, __FILE__, __LINE__))

// ----------------------------------------------------------------------------
// Function: log_init
// Purpose:  Opens the log files of this session and writes the opening
//           message. File names have the form engine_YYYY_MM_DD_HH_MM_SS.log;
//           a numeric suffix is appended if that name is already taken.
// Params:   none
// Returns:  nothing
// Reads:    the working directory and its parent directories
// Writes:   the log state; creates the log directories and files
// Notes:    Logging before log_init or after log_quit still reaches the
//           console, only the files are skipped.
// ----------------------------------------------------------------------------
void log_init(void);

// ----------------------------------------------------------------------------
// Function: log_quit
// Purpose:  Writes the closing message and closes the log files.
// Params:   none
// Returns:  nothing
// Reads:    the log state
// Writes:   the log state; closes the log files
// ----------------------------------------------------------------------------
void log_quit(void);

// ----------------------------------------------------------------------------
// Function: log_begin
// Purpose:  First half of a LOG_* macro: records the severity and call site
//           of the message that is about to be written.
// Params:   level - severity of the message
//           file  - source file of the call site, normally __FILE__
//           line  - source line of the call site, normally __LINE__
// Returns:  log_write, to be called immediately with the message format.
// Reads:    nothing
// Writes:   the pending call site in the log state
// ----------------------------------------------------------------------------
LogWriter log_begin(LogLevel level, const char *file, int line);

// ----------------------------------------------------------------------------
// Function: log_write
// Purpose:  Second half of a LOG_* macro: formats the message and writes it,
//           prefixed with a timestamp, the severity and the call site, to
//           the console and to every open log file. A LOG_LEVEL_FATAL
//           message terminates the process with EXIT_FAILURE.
// Params:   format - printf-style format string
//           ...    - values referenced by the format string
// Returns:  nothing
// Reads:    the log state, the system clock
// Writes:   the console streams and the open log files
// Notes:    Messages longer than LOG_MESSAGE_CAPACITY - 1 characters are
//           truncated.
// ----------------------------------------------------------------------------
void log_write(const char *format, ...);

#endif
