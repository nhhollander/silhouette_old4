/*!
 *  @file src/util/xdg.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "util/xdg.hpp"

#include "util/log.hpp"

#include <cstdlib>
#include <sys/stat.h>
#include <string.h>

std::string util::xdg::xdg_data_home() {
    // Return environment variable if set
    const char* env = std::getenv("XDG_DATA_HOME");
    if(env != nullptr) {
        return std::string(env);
    }
    // Return the default value
    std::string def_val;
    def_val += std::getenv("HOME");
    def_val += "/.local/.share";
    return def_val;
}

std::string util::xdg::xdg_config_home() {
    // Return environment variable if set
    const char* env = std::getenv("XDG_CONFIG_HOME");
    if(env != nullptr) {
        return std::string(env);
    }
    // Return the default value
    std::string def_val;
    def_val += std::getenv("HOME");
    def_val += "/.config";
    return def_val;
}

std::string util::xdg::xdg_cache_home() {
    // Return environment varible if set
    const char* env = std::getenv("XDG_CACHE_HOME");
    if(env != nullptr) {
        return std::string(env);
    }
    // Return the default value
    std::string def_val;
    def_val += std::getenv("HOME");
    def_val += "./cache";
    return def_val;
}