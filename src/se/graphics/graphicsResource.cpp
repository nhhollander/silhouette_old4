/*!
 *  @file include/se/graphics/graphicsResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsResource.hpp"

// ============================
// == DEBUG/HELPER FUNCTIONS ==
// ============================

#define _GRS_CASE_(n) case(GraphicsResourceState::n): return #n;
const char* se::graphics::graphics_resource_state_name(GraphicsResourceState state) {
    switch(state) {
        _GRS_CASE_(NOT_LOADED)
        _GRS_CASE_(LOADING)
        _GRS_CASE_(ERROR)
        _GRS_CASE_(CHILD_ERROR)
        _GRS_CASE_(LOADED)
        default: return "<invalid GraphicsResourceState>";
    }
}

#define _GRT_CASE_(n) case(GraphicsResourceType::n): return #n;
const char* se::graphics::graphics_resource_type_name(GraphicsResourceType type) {
    switch(type) {
        _GRT_CASE_(TEST_TYPE)
        default: return "<invalid GraphicsResourceType>";
    }
}