/*!
 *  @file include/se/graphics/graphicsResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsResource.hpp"

#include "util/hash.hpp"

using namespace se::graphics;

// ============================
// == DEBUG/HELPER FUNCTIONS ==
// ============================

#define _GRS_CASE_(n) case(GraphicsResourceState::n): return #n;
const char* graphics_resource_state_name(GraphicsResourceState state) {
    switch(state) {
        _GRS_CASE_(NOT_LOADED)
        _GRS_CASE_(LOADING)
        _GRS_CASE_(ERROR)
        _GRS_CASE_(CHILD_ERROR)
        _GRS_CASE_(LOADED)
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

void GraphicsResource::add_resource_dependency(GraphicsResource* resource) {
    this->resource_dependencies.push_back(resource);
    resource->add_resource_dependent(this);
}

// =====================
// == PRIVATE METHODS ==
// =====================

void GraphicsResource::add_resource_dependent(GraphicsResource* resource) {
    this->resource_dependents.push_back(resource);
}

bool GraphicsResource::resource_unloadable() {
    for(auto it : this->resource_dependents) {
        if(it->resource_state != GraphicsResourceState::NOT_LOADED) {
            return false;
        }
    }
    return true;
}