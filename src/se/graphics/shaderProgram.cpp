/*!
 *  @file src/se/graphics/shaderProgram.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/shaderProgram.hpp"

#include "se/engine.hpp"
#include "se/graphics/shader.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/log.hpp"
#include "util/hash.hpp"
#include "util/debugstrings.hpp"

#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace se::graphics;

// ====================
// == STATIC METHODS ==
// ====================

std::map<uint32_t, ShaderProgram*> ShaderProgram::cache;

ShaderProgram* ShaderProgram::get_program(se::Engine* engine, const char* vshader, const char* fshader) {
    DEBUG("Retrieving shader program [%p:%s:%s]", engine, vshader, fshader);
    uint32_t hash = util::hash::ejenkins("%p:%s:%s", engine, vshader, fshader);

    // Check the cache
    auto check = ShaderProgram::cache.find(hash);
    if(check != ShaderProgram::cache.end()) {
        DEBUG("Found shader [%p:%s:%s] in cache", engine, vshader, fshader);
        return check->second;
    }

    // Create a new program
    ShaderProgram* program = new ShaderProgram(engine, vshader, fshader);
    ShaderProgram::cache.insert(std::pair(hash, program));
    return program;
}

// =====================
// == PRIVATE METHODS ==
// =====================

const char* shader_state_name(ShaderState state) {
    switch(state) {
        case ShaderState::LOADING: return "LOADING";
        case ShaderState::READY: return "READY";
        case ShaderState::ERROR: return "ERROR";
        default: return "<invalid ShaderState>";
    }
}

const char* shader_program_state_name(ShaderProgramState state) {
    switch(state) {
        case ShaderProgramState::LOADING: return "LOADING";
        case ShaderProgramState::READY: return "READY";
        case ShaderProgramState::CHILD_ERROR: return "CHILD_ERROR";
        case ShaderProgramState::ERROR: return "ERROR";
        default: return "<invalid ShaderProgramState>";
    }
}

ShaderProgram::ShaderProgram(se::Engine* engine, const char* vsname, const char* fsname) {
    this->engine = engine;
    this->vsname = strdup(vsname);
    this->fsname = strdup(fsname);

    // Get the shader name
    this->name = std::string();
    this->name += vsname;
    this->name += ".vert:";
    this->name += fsname;
    this->name += ".frag";

    // Get the shaders
    this->vshader = Shader::get_shader(engine, vsname, GL_VERTEX_SHADER);
    this->fshader = Shader::get_shader(engine, fsname, GL_FRAGMENT_SHADER);

    ShaderState vstate = this->vshader->wait_for_loading();
    ShaderState fstate = this->fshader->wait_for_loading();

    if(vstate == ShaderState::ERROR || fstate == ShaderState::ERROR) {
        ERROR("[%s] Child shader is in error state! [vert: %s frag: %s]",
            this->name.c_str(),
            shader_state_name(vstate),
            shader_state_name(fstate));
        this->state = ShaderProgramState::CHILD_ERROR;
        return;
    }

    // Submit to the linking queue
    std::function job = [this](){this->link();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

void ShaderProgram::link() {

    auto start_time = std::chrono::system_clock::now();

    DEBUG("Linking Shader Program [%s]", this->name.c_str());

    this->gl_program = glCreateProgram();
    if(this->gl_program == 0) {
        GLenum error = glGetError();
        ERROR("[%s] Failed to create new program [%s: %s]",
            this->name.c_str(),
            util::string::gl_error_name(error),
            util::string::gl_error_desc(error));
        this->state = ShaderProgramState::ERROR;
        return;
    }

    glAttachShader(this->gl_program, this->vshader->get_gl_id());
    glAttachShader(this->gl_program, this->fshader->get_gl_id());

    glLinkProgram(this->gl_program);

    GLint success = GL_FALSE;
    glGetProgramiv(this->gl_program, GL_LINK_STATUS, &success);
    if(success != GL_TRUE) {
        // Get and print the entire linking log
        int length;
        int max_length;
        glGetProgramiv(this->gl_program, GL_INFO_LOG_LENGTH, &max_length);
        char* log = new char[max_length];
        glGetProgramInfoLog(this->gl_program, max_length, &length, log);
        ERROR("[%s] Failed to link program:\n%s", this->name.c_str(), log);
        delete[] log;
        this->state = ShaderProgramState::ERROR;
        return;
    }

    // Success
    this->state = ShaderProgramState::READY;

    auto end_time = std::chrono::system_clock::now();
    auto duration = end_time - start_time;
    uint64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    float milliseconds = time / 1000000;
    DEBUG("[%s] Program linking took %.3fms.",
        this->name.c_str(), milliseconds);
}

// ====================
// == PUBLIC METHODS ==
// ====================

ShaderProgramState ShaderProgram::get_state() {
    return this->state;
}

ShaderProgramState ShaderProgram::wait_for_loading() {
    while(this->state == ShaderProgramState::LOADING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return this->state;
}