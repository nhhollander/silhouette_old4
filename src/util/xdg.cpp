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

std::string util::xdg::xdg_runtime_dir() {
    // Return environment variable if set
    const char* env = std::getenv("XDG_RUNTIME_DIR");
    if(env != nullptr) {
        return std::string(env);
    }
    #ifdef ENABLE_XDG_RUNTIME_FALLBACK
    /* The XDG specification doesn't like it when the runtime directory is not
     * set, because the requirements are too specific to have a generic fallback
     * directory.  To deal with this, a special temporary runtime directory must
     * be created to contain runtime files.  According to the specification,
     * this folder must have permission 700 (rwx on owner, no permissions for
     * other users) */
    WARN("'$XDG_RUNTIME_DIR' not set - Falling back to cache");
    std::string xdg_cache = util::xdg::xdg_cache_home();
    std::string new_runtime_folder = xdg_cache;
    new_runtime_folder += "/xdg_runtime_tmp";
    // Check if the fallback directory already exists
    struct stat info;
    if(stat(new_runtime_folder.c_str(), &info) != 0) {
        goto failure;
    } else if(info.st_mode & S_IFDIR) {
        goto success;
    } else {
        ERROR("What the heck?  There's a file named 'xdg_runtime_tmp' where the XDG runtime fallback directory is supposed to go.");
        goto failure;
    }
    if(mkdir(new_runtime_folder.c_str(), 700) == -1) {
        goto failure;
    } else {
        goto success;
    }

    failure:
        ERROR("Failed to create fallback XDG runtime directory! Falling back to bare cache dir [THIS IS BAD!] [%i:%s]",
            errno, strerror(errno));
        return xdg_cache;

    success:
        return new_runtime_folder;
    #else
    FATAL("'$XDG_RUNTIME_DIR not set - unable to determine runtime file directory");
    return std::string("<invalid>");
    #endif
    
}