/*!
 *  @file src/util/dirs.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/dirs.hpp"

#include "util/log.hpp"

#include <cstdlib>
#include <sys/stat.h>
#include <string.h>

std::string util::dirs::user_data(const char* appname) {
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
std::string util::dirs::user_data(std::string appname) {
    return util::dirs::user_data(appname.c_str());
}

std::string util::dirs::temp_data(const char* appname) {
    std::string value = "/tmp/";
    value += appname;
    return value;
}
std::string util::dirs::temp_data(std::string appname) {
    return util::dirs::temp_data(appname.c_str());
}

std::string util::dirs::cache_data(const char* appname) {
    std::string value = "/var/tmp/";
    value += appname;
    return value;
}
std::string util::dirs::cache_data(std::string appname) {
    return util::dirs::cache_data(appname.c_str());
}

std::string util::dirs::app_data(const char* appname) {
    std::string value = "/opt/";
    value += appname;
    return value;
}
std::string util::dirs::app_data(std::string appname) {
    return util::dirs::app_data(appname.c_str());
}