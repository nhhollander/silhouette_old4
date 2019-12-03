/*!
 *  @file include/se/util/hash.hpp
 * 
 *  Static hashing helper functions.  This file provides basic **non-
 *  cryptographic** hashing functions.
 * 
 *  Copyright 2018 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_UTIL_HASH_H_
#define _SE_UTIL_HASH_H_

#include <cstdint>
#include <cstdlib>

namespace se::util::hash {

    /*!
     *  Jenkins' one-at-a time hash.
     * 
     *  This is a fast hashing function with a collision rate somewhere between
     *  that of FNV-1a and Murmur3. [Here's a pretty good comparison](
     *  https://research.neustar.biz/2011/12/29/choosing-a-good-hash-function-part-2/).
     * 
     *  @param data Pointer to the data to be hashed.
     *  @param len  Length of the input data in bytes
     * 
     *  @return A 32 bit hash of the input data
     */
    uint32_t jenkins(const void* data, size_t len);

    /*!
     *  Extended jenkins.
     * 
     *  This is a wrapper for `jenkins()` that calculates the hash based on a
     *  series of `printf()` style arguments.  This function makes calculating
     *  the hash of compound data significantly easier.
     * 
     *  See `jenkins()` for more information.
     * 
     *  @param format   `printf()` style format string.
     * 
     *  @return A 32 bit jenkins hash of the input data.
     */
    uint32_t ejenkins(const char* format, ...);

}

#endif