/*!
 *  @file src/util/hash.cpp
 * 
 *  Copyright 2018 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/hash.hpp"

#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include "util/log.hpp"

/// Extended hash buffer size
#define LINE_BUFFER_SIZE 4096*2

uint32_t util::hash::jenkins(const void* data, size_t len) {
    uint32_t hash = 0;
    // Digest the next byte
    for(size_t i = 0; i < len; i++) {
        hash += ((uint8_t*) data)[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    // Give the hash one final mixing about
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

uint32_t util::hash::ejenkins(const char* format, ...) {
    // Prepare the variable arguments
    va_list args;
    va_start(args, format);
    // Get the message buffer size
    int message_length = vsnprintf(nullptr, 0, format, args);
    // Rewind the variable arguments
    va_start(args, format);
    // Create the message buffer
    char buf[message_length + 1];
    // Generate the digestable string
    int actual_message_length = vsnprintf(buf, message_length + 1, format, args);
    // Check for truncation
    if(actual_message_length > message_length) {
        // Potentially incomplete hash
        WARN("Extended hash calculation truncated!  Input data of length [%i] exceeds limit of [%i]",
            actual_message_length, message_length);
    }
    // Get the length
    int input_len = strlen(buf);
    // Hash the data
    return jenkins((uint8_t*) &buf, input_len);
}