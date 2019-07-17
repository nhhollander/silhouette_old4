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

se::graphics::GraphicsController::GraphicsController(se::Engine* engine) {
    DEBUG("Initializing new graphics controller");
    this->engine = engine;

    // Initialize SDL2 in OpenGL mode
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        FATAL("Failed to initialize SDL2 [%s]", SDL_GetError());
        return;
    }

    // Configure the OpenGL version
    int gl_major = this->engine->config->get_int("gl.major", -1);
    int gl_minor = this->engine->config->get_int("gl.minor", -1);
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
    bool vsync = this->engine->config->get_bool("gl.vsync",false);
    if(SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0) {
        ERROR("Failed to configure vsync [%s]", SDL_GetError());
    }

}