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

}

#endif