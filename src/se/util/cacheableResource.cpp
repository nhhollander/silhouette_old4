/*!
 *  @file src/se/util/cacheableReource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/util/cacheableResource.hpp"

#include "se/util/log.hpp"

using namespace se::util;

std::map<uint32_t, CacheableResource*> CacheableResource::resource_cache;

std::mutex CacheableResource::cache_insertion_lock;

CacheableResource* CacheableResource::find_resource(uint32_t hash) {
    auto find = CacheableResource::resource_cache.find(hash);
    if(find == CacheableResource::resource_cache.end()) {
        return nullptr;
    }
    return find->second;
}

bool CacheableResource::cache_resource(CacheableResource* resource) {
    CacheableResource::cache_insertion_lock.lock();
    uint32_t hash = resource->resource_id();
    // TODO: Duplicate check necessary?
    if(CacheableResource::find_resource(hash) != nullptr) {
        WARN("Duplicate cach insertion [%08X:%s]", hash,
            resource->resource_name().c_str());
        CacheableResource::cache_insertion_lock.unlock();
        return false;
    }
    CacheableResource::resource_cache.insert(std::pair(hash, resource));
    CacheableResource::cache_insertion_lock.unlock();
    return true;
}

bool CacheableResource::decache_resource(CacheableResource* resource) {
    uint32_t hash = resource->resource_id();
    if(CacheableResource::resource_cache.erase(hash) == 0) {
        WARN("Attempted to remove nonexistant resource [%08X:%s]", hash, resource->resource_name());
        return false;
    }
    return true;
}

bool CacheableResource::decache_resource(uint32_t hash) {
    if(CacheableResource::resource_cache.erase(hash) == 0) {
        WARN("Attempted to remove nonexistant resource (by hash) [%08X]", hash);
        return false;
    }
    return true;
}