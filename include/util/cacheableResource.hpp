/*!
 *  @file include/util/cacheableResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_CACHEABLERESOURCE_H_
#define _UTIL_CACHEABLERESOURCE_H_

#include <cstdint>
#include <map>
#include <mutex>

namespace util {

    /*!
     *  Cacheable Resource.
     * 
     *  Cacheable resources are objects which may be added to a global lookup
     *  table and retrieved from anywhere in the application at any time via
     *  a unique identifier.
     */
    class CacheableResource {

        private:

            /*!
             *  Resource Cache.
             * 
             *  This is where the cacheable resources live.
             */
            static std::map<uint32_t, CacheableResource*> resource_cache;

            /*!
             *  Resource Cache Mutex.
             * 
             *  Prevents race condition on resource insertion.
             */
            static std::mutex cache_insertion_lock;

        public:

            /*!
             *  Retrieve the cache ID/Hash.
             * 
             *  This identifier should be a unique value to a particular class
             *  construction.  For example, a `Texture` object which loads the
             *  image file `mytexture.png` might generate this identifier by
             *  calculating the hash of `Texture:mytexture.png`.  This allows
             *  texture to be retrieved from the cache using only information
             *  that is already known.
             * 
             *  This value is also known as the "hash" in other parts of the
             *  application.
             */
            virtual uint32_t resource_id() = 0;

            /*!
             *  Retrieve resource name.
             * 
             *  This value is primarily used for debugging.
             */
            virtual std::string resource_name() = 0;

            /*!
             *  Search for an entry in the resource cache.
             * 
             *  If no entry exists in the cache with the given ID, this method
             *  will return `nullptr`.
             */
            static CacheableResource* find_resource(uint32_t hash);

            /*!
             *  Add resource to the cache by resource pointer.
             */
            static bool cache_resource(CacheableResource* resource);

            /*!
             *  Remove resoure from the cache by resourece pointer.
             */
            static bool decache_resource(CacheableResource* resource);

            /*!
             *  Remove resource from the cache by hash.
             */
            static bool decache_resource(uint32_t hash);


    };


}

#endif