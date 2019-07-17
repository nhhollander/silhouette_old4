/*!
 *  @file include/util/xdg.hpp
 * 
 *  Definitions for functions that provide information about various important
 *  system directories, including global and user specific application data.
 * 
 *  Partially compatible with the XDG base directories standard.
 *  https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_DIRS_H_
#define _UTIL_DIRS_H_

#include <string>

namespace util::dirs {

    /*!
     *  Get the user data base folder.
     * 
     *  Data that is specific to a single user on the system should be stored in
     *  this folder.
     */
    std::string user_data();

    /*!
     *  Get the temp data base folder.
     * 
     *  Data that is used for runtime file storage.  Don't expect things saved
     *  here to be there next time the program is run.
     */
    std::string temp_data();

    /*!
     *  Get the cache data base folder.
     * 
     *  Data that would be faster to read from the local disk instead of
     *  re-downloading or re-generating next time it's needed.  Stuff stored
     *  here will probably still be available next time the program is run, but
     *  it's not guarenteed.
     */
    std::string cache_data();

    /*!
     *  Get the application data base folder.
     * 
     *  Read-only application resources.  You shouldn't be modifying files in
     *  this directory.
     */
    std::string app_data();

}

#endif