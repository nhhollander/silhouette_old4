/*!
 *  @file src/se/input/inputController.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/input/inputController.hpp"

#include "se/engine.hpp"

#include "se/util/config.hpp"
#include "se/util/log.hpp"

#include <chrono>
#include <SDL2/SDL.h>

using namespace se::input;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void InputController::input_thread_main() {
    se::util::log::set_thread_name("INPUT");
    INFO("Hello from the input thread!");

    // Main input loop
    SDL_Event input_event;
    while(this->engine->threads_run) {

        if(SDL_PollEvent(&input_event)) {
            for(auto handler : this->handlers) {
                handler(input_event);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

    }

    DEBUG("Input thread terminated");

}

void InputController::quit_handler(SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        //if(event.key.keysym.sym == SDLK_ESCAPE) {
        //    this->engine->threads_run = false;
        //}
    } else if(event.type == SDL_QUIT) {
        this->engine->threads_run = false;
    }
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

InputController::InputController(se::Engine* engine) {
    DEBUG("Initializing new input controller");
    this->engine = engine;

    // Start the input thread
    this->input_thread = std::thread(&InputController::input_thread_main, this);

    // Debug handler - should be removed eventually
    InputHandler handler = [this](SDL_Event event){this->quit_handler(event);};
    this->register_handler(handler);
}

InputController::~InputController() {

    if(this->input_thread.joinable()) {
        DEBUG("Waiting for input thread to exit");
        this->input_thread.join();
    }

}

void InputController::register_handler(InputHandler handler) {
    this->handlers.push_back(handler);
}