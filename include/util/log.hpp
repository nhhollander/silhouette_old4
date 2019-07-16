/*!
 *  @file include/util/log.hpp
 * 
 *  Static logging helper library.  This basic helper library provides a fast,
 *  configurable, and easy to use logging interface.  Other major logging
 *  systems were too complicated, bulky, or would have added more dependencies
 *  to this project, something I have been trying to avoid.
 * 
 *  Copyright 2018 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_LOG_H_
#define _UTIL_LOG_H_

// Log levels

/*!
 *  Log level used for debug events.
 * 
 *  Messages with this level do not generally contain important runtime
 *  information, but can be valuable when debugging the program, or trying to
 *  find the source of a problem.
 */
#define LL_DEBUG 0x00
/*!
 *  Log level used for information messages.
 * 
 *  Messages with this level contain information relevant to the application's
 *  state during runtime, and generally reflect actions performed by or relating
 *  to the user.
 */
#define LL_INFO  0x02
/*!
 *  Log level used for warning messages.
 * 
 *  Messages with this level contain information about potential issues in a
 *  game or the engine, although the application should be able to continue
 *  operating without major issue.
 */
#define LL_WARN  0x03
/*!
 *  Log level used for error messages.
 * 
 *  Messages with this level contain information about serious issues with the
 *  engine or entities in a game, that are most likely noticeable to the user,
 *  and could cause application instability.
 */
#define LL_ERROR 0x04
/*!
 *  Log level used for fatal error messages.
 * 
 *  Messages with this level contain information about extremely serious issues
 *  with the engine or application that prevent continued operation of the
 *  program.  This type of message should only be generated when a failure is so
 *  severe the program must terminate.
 */
#define LL_FATAL 0x05

// Make sure the UTIL_LOG_PATH_OFFSET hack of a workaround is in place
#ifndef UTIL_LOG_PATH_OFFSET
    /*!
     *  Because gcc expands the `__FILE__` macro to include the path as given on
     *  the command line, and cmake uses absolute paths, a lot of unnecessary
     *  information is included.  This offset strips out the first n characters
     *  of the absolute path so that only a relative path remains.
     */
    #define UTIL_LOG_PATH_OFFSET 0
    #warning UTIL_LOG_PATH_OFFSET was not set!  See doc/seutil.md for more info.
#endif

/// Log a debug level message
#define DEBUG(message, ...) util::log::log(LL_DEBUG, __FILE__ + UTIL_LOG_PATH_OFFSET, __LINE__, __func__, message, ##__VA_ARGS__)
/// Log an info level message
#define INFO(message, ...)  util::log::log(LL_INFO,  __FILE__ + UTIL_LOG_PATH_OFFSET, __LINE__, __func__, message, ##__VA_ARGS__)
/// Log a warning level message
#define WARN(message, ...)  util::log::log(LL_WARN,  __FILE__ + UTIL_LOG_PATH_OFFSET, __LINE__, __func__, message, ##__VA_ARGS__)
/// Log an error level message
#define ERROR(message, ...) util::log::log(LL_ERROR, __FILE__ + UTIL_LOG_PATH_OFFSET, __LINE__, __func__, message, ##__VA_ARGS__)
/// Log a fatal error level message
#define FATAL(message, ...) util::log::log(LL_FATAL, __FILE__ + UTIL_LOG_PATH_OFFSET, __LINE__, __func__, message, ##__VA_ARGS__)

namespace util::log {

    /*!
     *  Log a message.
     * 
     *  This function formats and logs the message and path information given
     *  via expansion of the above macros.
     * 
     *  *Note: This function is intended only to be called via expansion of the
     *  logging macros*
     * 
     *  @param level    Log level.
     *  @param fname    Name of source file where the message was generated
     *  @param line     Line number where the message was generated
     *  @param func     Function or method in which the message was generated
     *  @param message  Base logging message and/or `vsnprintf` format string.
     *  @param ...      Variable arguments to be parsed by `vsnprintf`
     */
    void log(char level, const char* fname, int line, const char* func, const char* message, ...);

    /*!
     *  Set the miniumum log level for printing.
     * 
     *  This function sets the minimum level for log messages in order to have
     *  those messages printed to the console.  Messages with lower levels may
     *  still be written to the log file if that option is enabled.
     * 
     *  *Note that this is a global option, and will affect all parts of the
     *  program that rely on the logger.*
     * 
     *  @param level    Minimum log level.
     */
    void set_min_print_level(char level);

    /*!
     *  Set the minimum log level for writing to the log file.
     * 
     *  This function sets the minimum legel for log messages in order to have
     *  those messages saved to the log file, if the file is open.  Messages
     *  with lower levels may still be written to the console.
     * 
     *  *Note that this is a global option, and will affect all parts of the
     *  program that rely on the logger.*
     * 
     *  @param level    Minimum log level.
     */
    void set_min_logfile_level(char level);

    /*!
     *  Open Log File.
     * 
     *  This function opens the specified file as the log file.  All messages
     *  generated during runtime will be otuput to this file.
     * 
     *  If the file can not be opened, a warning will be generated, and this
     *  function will return `false`.
     * 
     *  **Warning: The contents of this file will be overwritten.**
     * 
     *  @param fname    Name of the file to write to.
     * 
     *  @return `true` if the file is opened, `false` if something goes wrong.
     */
    bool open_log_file(const char* fname);

    /*!
     *  Set the thread name.
     * 
     *  This makes debugging easier, as each message will contain the name of
     *  the thread that it was sent from.
     * 
     *  @param  name    Name of the thread.
     */
    void set_thread_name(const char* name);

}

#endif