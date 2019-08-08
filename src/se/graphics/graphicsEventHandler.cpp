/*!
 *  @file src/se/graphics/eventHandlers.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsEventHandler.hpp"

#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/input/inputController.hpp"

#include "util/log.hpp"
#include "util/config.hpp"

using namespace se::graphics;

// ===================
// == PUBLIC MBMERS ==
// ===================

GraphicsEventHandler::GraphicsEventHandler(GraphicsController* parent) {
    this->parent = parent;

    this->parent->engine->input_controller->register_handler(
        [this](SDL_Event event){this->window_resize(event);});
}

// =====================
// == PRIVATE MEMBERS ==
// =====================

void GraphicsEventHandler::window_resize(SDL_Event event) {
    if(event.type == SDL_WINDOWEVENT) {
        if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int dimx = event.window.data1;
            int dimy = event.window.data2;
            DEBUG("Window resized to [%ix%i]", dimx, dimy);
            this->parent->engine->config->set("window.dimx", dimx);
            this->parent->engine->config->set("window.dimy", dimy);
            this->parent->submit_graphics_task([dimx,dimy](){
                glViewport(0, 0, dimx, dimy);
            });
        }
    }
}