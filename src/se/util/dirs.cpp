/*!
 *  @file src/se/util/dirs.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/util/dirs.hpp"

#include "se/util/log.hpp"

#include <cstdlib>
#include <sys/stat.h>
#include <string.h>

std::string se::util::dirs::user_data() {
    // Check for debug override
    const char* override = std::getenv("SE_DEBUG_USERDATA");
    if(override != nullptr) {
        return std::string(override);
    }

    const char* appname = std::getenv("SE_APPNAME");
    // Return environment variable if set
    const char* env = std::getenv("XDG_DATA_HOME");
    if(env != nullptr) {
        std::string value(env);
        value += "/";
        value += appname;
        return value;
    }
    // Return the default value
    std::string def_val = std::getenv("HOME");
    def_val += "/.local/.share";
    return def_val;
}
std::string se::util::dirs::temp_data() {
    // Check for debug override
    const char* override = std::getenv("SE_DEBUG_TEMPDATA");
    if(override != nullptr) {
        return std::string(override);
    }

    const char* appname = std::getenv("SE_APPNAME");
    std::string value = "/tmp/";
    value += appname;
    return value;
}

std::string se::util::dirs::cache_data() {
    // Check for debug override
    const char* override = std::getenv("SE_DEBUG_CACHEDATA");
    if(override != nullptr) {
        return std::string(override);
    }

    const char* appname = std::getenv("SE_APPNAME");
    std::string value = "/var/tmp/";
    value += appname;
    return value;
}

std::string se::util::dirs::app_data() {
    // Check for debug override
    const char* override = std::getenv("SE_DEBUG_APPDATA");
    if(override != nullptr) {
        return std::string(override);
    }

    const char* appname = std::getenv("SE_APPNAME");
    std::string value = "/opt/";
    value += appname;
    return value;
}