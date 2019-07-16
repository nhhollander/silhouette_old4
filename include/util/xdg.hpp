/*!
 *  @file include/util/xdg.hpp
 * 
 *  Definitions for the XDG base directory specification helper utility, in
 *  accordance with the standards defined at
 *  https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_XDG_H_
#define _UTIL_XDG_H_

#include <string>

namespace util::xdg {

    /*!
     *  Get the user-specific data base directory.
     * 
     *  > There is a single base directory relative to which user-specific data
     *  > files should be written. This directory is defined by the environment
     *  > variable `$XDG_DATA_HOME`. 
     */
    std::string xdg_data_home();

    /*
     *  Get the user-specific configuration base directory.
     * 
     *  > There is a single base directory relative to which user-specific
     *  > configuration files should be written. This directory is defined by
     *  > the environment variable `$XDG_CONFIG_HOME`.
     */
    std::string xdg_config_home();

    /*
     *  Get the user-specific cache base directory.
     * 
     *  > There is a single base directory relative to which user-specific
     *  > non-essential (cached) data should be written. This directory is
     *  > defined by the environment variable `$XDG_CACHE_HOME`. 
     */
    std::string xdg_cache_home();

    /*!
     *  Get the user-specific runtime base directory.
     * 
     *  Remember to update the modified time on files in this directory at least
     *  once every 6 hours and/or set the sticky bit to prevent them from being
     *  cleaned up.
     * 
     *  This function has two operating modes.  If  the flag 
     *  `ENABLE_XDG_RUNTIME_FALLBACK` is specified when building, this method
     *  will attempt to utilize or create a runtime folder under the xdg cache
     *  folder.  While this *should* be safe, it is possible that permissions on
     *  the cache fallback folder could be incorrect, or the filesystem on which
     *  the cache resides could not support required features.
     *
     *  > There is a single base directory relative to which user-specific
     *  > runtime files and other file objects should be placed. This directory
     *  > is defined by the environment variable `$XDG_RUNTIME_DIR`. 
     */
    std::string xdg_runtime_dir();

}

#endif