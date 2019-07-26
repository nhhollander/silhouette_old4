/*!
 *  @file include/se/graphics/graphicsResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsResource.hpp"

#include "util/hash.hpp"
#include "util/log.hpp"

using namespace se::graphics;

// ============================
// == DEBUG/HELPER FUNCTIONS ==
// ============================

#define _GRS_CASE_(n) case(GraphicsResourceState::n): return #n;
const char* graphics_resource_state_name(GraphicsResourceState state) {
    switch(state) {
        _GRS_CASE_(NOT_LOADED)
        _GRS_CASE_(LOADING)
        _GRS_CASE_(LOADED)
        _GRS_CASE_(ERROR) // Not `ERROR` from util::log
        default: return "<invalid GraphicsResourceState>";
    }
}

// ====================
// == STATIC MEMBERS ==
// ====================

std::map<uint32_t,GraphicsResource*> GraphicsResource::resource_cache;

GraphicsResource* GraphicsResource::get_resource(uint32_t unique_id) {
    auto search = GraphicsResource::resource_cache.find(unique_id);
    if(search == GraphicsResource::resource_cache.end()) {
        return nullptr;
    } else {
        return search->second;
    }
}

// ====================
// == PUBLIC METHODS ==
// ====================

GraphicsResourceState GraphicsResource::get_resource_state() {
    return this->resource_state;
}

void GraphicsResource::increment_active_users() {
    if(this->active_users == 0) {
        this->load_();
    }
    this->active_users += 1;
}

void GraphicsResource::decrement_active_users() {
    if(this->active_users == 0) {
        WARN("Attempted to decrement active users on resource with zero users!");
    } else {
        this->active_users -= 1;
        if(this->active_users == 0) {
            /* TODO: Add logic to determine when it's actually necessary to
            unload resources to prevent excessive loading and unloading. */
            this->unload_();
        }
    }
}

// =======================
// == PROTECTED METHODS ==
// =======================

GraphicsResource::GraphicsResource(uint32_t unique_id) {
    this->resource_cache.insert(std::pair(unique_id, this));
}