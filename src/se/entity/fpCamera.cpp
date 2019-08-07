/*!
 *  @file src/se/entity/fpCamera.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity/fpCamera.hpp"

#include "se/engine.hpp"
#include "se/input/inputController.hpp"

#include "util/log.hpp"
#include "util/debugstrings.hpp"

using namespace se::entity;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void FPCamera::sdl_event_handler(SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        // TODO
    } else if(event.type == SDL_MOUSEMOTION) {
        int x = event.motion.xrel;
        int y = event.motion.yrel;
        this->rz += (x / 600.0);
        this->rx += (y / 600.0);
    }
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

FPCamera::FPCamera(se::Engine* engine) : Camera(engine) {
    std::function handler = [this](SDL_Event event){this->sdl_event_handler(event);};
    engine->input_controller->register_handler(handler);
}