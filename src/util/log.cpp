/*!
 *  @file src/util/log.cpp
 * 
 *  Copyright 2018 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/log.hpp"

#include <cstdarg>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string>
#include <string.h>
#include <pthread.h>

/// Message buffer size
#define LINE_BUFFER_SIZE 4096*2

/// Gets the thread ID
#define TID syscall(SYS_gettid)

/// Minimum log level for printing to the console
char min_print_level = LL_DEBUG;

/// Minimum log level for writing to the log file
char min_logfile_level = LL_DEBUG;

/// Log file
FILE* logfile = nullptr;

/// Thread name
const thread_local char* thread_name = "-";

/*!
 *  Retrieve the log format for a given level.
 * 
 *  This function will return a `printf` style format string to be used for
 *  printing messages to the console.  Each level should have a unique style
 *  that can be used to quickly identify the severity of a message at a glance.
 * 
 *  @param level    The level to get the format for
 * 
 *  @return A format string if `level` is valid, otherwise a default message
 *  format.
 */
const char* getformat(char level) {
    switch(level) {
        case LL_DEBUG:  return "[%i][%s][\033[35mDEBUG\033[0m][\033[38;5;245m%s\033[0m:\033[32m%i\033[0m:\033[38;5;245m%s\033[0m] %s\033[0m\n";
        case LL_INFO:   return "[%i][%s][\033[38;5;27mINFO \033[0m][\033[38;5;245m%s\033[0m:\033[32m%i\033[0m:\033[38;5;245m%s\033[0m] %s\033[0m\n";
        case LL_WARN:   return "[%i][%s][\033[33mWARN \033[0m][\033[38;5;245m%s\033[0m:\033[32m%i\033[0m:\033[38;5;245m%s\033[0m] %s\033[0m\n";
        case LL_ERROR:  return "[%i][%s][\033[31mERROR\033[0m][\033[38;5;245m%s\033[0m:\033[32m%i\033[0m:\033[38;5;245m%s\033[0m] %s\033[0m\n";
        case LL_FATAL:  return "\033[41m[%i][%s][FATAL][%s:%i:%s] %s\033[0m\n";
        default: return "[%i][%s][\033[32m?????\033[0m][%s:%i:%s] %s\033[0m\n";
    };
}

void util::log::log(char level, const char* fname, int line, const char* func, const char* message, ...) {
    // Check if the message is filtered
    if(level < min_print_level && level < min_logfile_level) {
        // Message will not be printed or written to file - discard it
        return;
    }
    // Get the variable arguments
    va_list args;
    va_start(args, message);
    char buf[LINE_BUFFER_SIZE];
    // Create the output message
    int message_length = vsnprintf(buf, LINE_BUFFER_SIZE, message, args);
    // Check if print to console
    if(level >= min_print_level) {
        // Print the message
        printf(getformat(level), TID, thread_name, fname, line, func, buf);
        // Check the print count
        if(message_length > LINE_BUFFER_SIZE) {
            // The output message was truncated
            printf("\033[33mPrevious message truncated! (Message of length [%i] exceeds buffer size [%i])\033[0m\n",
                message_length, LINE_BUFFER_SIZE);
        }
    }
    // Check if print to file
    if(level >= min_logfile_level && logfile != nullptr) {
        // Print the message
        int write_len = fprintf(logfile, getformat(level), TID, thread_name, fname, line, func, buf);
        // Check the length
        if(write_len < 0) {
            // A write error occurred
            printf("\033[33mEncountered an error while writing to log file! [%i: %s]\033[0m\n",
                errno, strerror(errno));
            // Close the output
            fclose(logfile);
            logfile = nullptr;
        }
    }
}

void util::log::set_min_print_level(char level) {
    // Set the level
    min_print_level = level;
}

void util::log::set_min_logfile_level(char level) {
    // Set the level
    min_logfile_level = level;
}

bool util::log::open_log_file(const char* fname) {
    // Open the file
    logfile = fopen(fname, "w");
    // Check if the file opened
    if(logfile == nullptr) {
        // The file failed to open
        printf("\033[33mFailed to open log file! [%i: %s]\033[0m\n",
            errno, strerror(errno));
        return false;
    }
    return true;
}

void util::log::set_thread_name(const char* name) {
    // Save the name
    thread_name = strdup(name);

    // Truncate the name for pthread
    char* ptname = strdup(name);
    if(strlen(ptname) > 15) {
        ptname[15] = '\0';
        WARN("New thread name [%s] exceeds 16 character POSIX limit and has been truncated to [%s]", name, ptname);
    }
    // Set the pthread name (for reference on POSIX systems)
    pthread_setname_np(pthread_self(), ptname);
    delete[] ptname;

}