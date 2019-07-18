/*!
 *  @file src/se/graphics/graphicsController.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/graphicsController.hpp"

#include "se/engine.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

#include <chrono>
#include <typeinfo>

se::graphics::GraphicsController::GraphicsController(se::Engine* engine) {
    DEBUG("Initializing new graphics controller");
    this->engine = engine;

    // Start the graphics thread
    this->graphics_thread = std::thread(&se::graphics::GraphicsController::graphics_thread_main, this);
}

se::graphics::GraphicsController::~GraphicsController() {
    
    if(this->graphics_thread.joinable()) {
        DEBUG("Waiting for graphics thread to exit");
        this->graphics_thread.join();
    }

}

void se::graphics::GraphicsController::graphics_thread_main() {
    util::log::set_thread_name("RENDER");
    INFO("Hello from the render thread!");
    this->engine->config->get("render.fpscap")->add_change_handler(this);

    // Initialize SDL2 in OpenGL mode
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        FATAL("Failed to initialize SDL2 [%s]", SDL_GetError());
        return;
    }

    // Configure the OpenGL version
    int gl_major = this->engine->config->get_int("render.gl.major", -1);
    int gl_minor = this->engine->config->get_int("render.gl.minor", -1);
    if(gl_major == -1 || gl_minor == -1) {
        FATAL("Opengl version missing! [%i.%i]", gl_major, gl_minor);
        return;
    }
    DEBUG("Using OpenGL %i.%i", gl_major, gl_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create the window
    const char* title = this->engine->config->get_cstring("window.title");
    int dimx = this->engine->config->get_int("window.dimx", -1);
    int dimy = this->engine->config->get_int("window.dimy", -1);
    if(title == nullptr || dimx == -1 || dimy == -1) {
        FATAL("Window properties missing! [%s:%i:%i]", title, dimx, dimy);
        return;
    }
    this->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dimx, dimy, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(this->window == nullptr) {
        FATAL("Failed to create SDL2 window [%s]", SDL_GetError());
        return;
    }

    // Create the OpenGL context
    this->glcontext = SDL_GL_CreateContext(this->window);
    if(this->glcontext == nullptr) {
        FATAL("Failed to create OpenGL context [%s]", SDL_GetError());
        return;
    }

    // Configure the OpenGL instance
    bool vsync = this->engine->config->get_bool("render.vsync",false);
    if(SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0) {
        ERROR("Failed to configure vsync [%s]", SDL_GetError());
    }
    
    INFO("Graphics initialization complete");

    // Variables for FPS cap


    // Main render loop
    while(this->engine->threads_run) {
        
        this->render();
    }

    DEBUG("Render thread terminated");

}

void se::graphics::GraphicsController::render() {

    DEBUG("RENDER!");

}

void se::graphics::GraphicsController::handle_config_change(util::ConfigurationValue* value, util::Configuration* config) {
    if(strcmp(value->ref_name, "render.vsync") == 0) {
        
    }
}

void se::graphics::GraphicsController::recalculate_fps_limit(util::ConfigurationValue* value, util::Configuration* config) {
    DEBUG("%p %p %p", this, value, config);
    return;
    int fps_cap = value->int_;
    this->target_frame_time = 1000000000 / fps_cap;
}