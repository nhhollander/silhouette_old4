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
using namespace util;

// ====================
// == STATIC METHODS ==
// ====================

uint32_t get_shader_program_hash(
    const char* vshader, const char* vdefines,
    const char* fshader, const char* fdefines) {
    return util::hash::ejenkins("shaderprogram:%s:%s:%s:%s",
        vshader, vdefines, fshader, fdefines);
}

ShaderProgram* ShaderProgram::get_program(se::Engine* engine,
    const char* vshader, const char* vdefines,
    const char* fshader, const char* fdefines) {
    uint32_t hash = get_shader_program_hash(vshader, vdefines, fshader, fdefines);
    CacheableResource* resource = ShaderProgram::find_resource(hash);
    if(resource == nullptr) {
        DEBUG("Shader Program [%s:%s] not in cache :(", vshader, fshader);
        return new ShaderProgram(engine, vshader, vdefines, fshader, fdefines);
    }
    DEBUG("Found shader program [%s:%s] in cache!", vshader, fshader);
    return static_cast<ShaderProgram*>(resource);
}

thread_local unsigned int ShaderProgram::current_program = 0;

// =====================
// == PRIVATE METHODS ==
// =====================

ShaderProgram::ShaderProgram(se::Engine* engine, 
    const char* vsname, const char* vdefines,
    const char* fsname, const char* fdefines) {

    this->engine = engine;
    this->vsname = strdup(vsname);
    this->fsname = strdup(fsname);

    // Get the shader name
    this->name = std::string();
    this->name += vsname;
    this->name += ".vert:";
    this->name += fsname;
    this->name += ".frag";

    // Save defines
    this->vdefines = strdup(vdefines);
    this->fdefines = strdup(fdefines);

    this->cache_resource(this);
}

ShaderProgram::~ShaderProgram() {
    free((void*)this->vsname);
    free((void*)this->fsname);
    free((void*)this->vdefines);
    free((void*)this->fdefines);
}

void ShaderProgram::link() {

    ShaderState vstate = this->vshader->wait_for_loading();
    ShaderState fstate = this->fshader->wait_for_loading();

    if(vstate == ShaderState::ERROR || fstate == ShaderState::ERROR) {
        ERROR("[%s] Child shader is in error state! [vert: %s frag: %s]",
            this->name.c_str(),
            shader_state_name(vstate),
            shader_state_name(fstate));
        this->resource_state = LoadableResourceState::CHILD_ERROR;
        return;
    }

    auto start_time = std::chrono::system_clock::now();

    DEBUG("Linking Shader Program [%s]", this->name.c_str());

    this->gl_program = glCreateProgram();
    if(this->gl_program == 0) {
        GLenum error = glGetError();
        ERROR("[%s] Failed to create new program [%s: %s]",
            this->name.c_str(),
            util::string::gl_error_name(error),
            util::string::gl_error_desc(error));
        this->resource_state = LoadableResourceState::ERROR;
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
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }

    // Success
    this->resource_state = LoadableResourceState::LOADED;

    auto end_time = std::chrono::system_clock::now();
    auto duration = end_time - start_time;
    uint64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    float milliseconds = time / 1000000;
    DEBUG("[%s] Program linking took %.3fms.",
        this->name.c_str(), milliseconds);
}

void ShaderProgram::unlink() {
    glDeleteProgram(this->gl_program);
}

// ====================
// == PUBLIC METHODS ==
// ====================

LoadableResourceState ShaderProgram::wait_for_loading() {
    while(this->resource_state == LoadableResourceState::LOADING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return this->resource_state;
}

void ShaderProgram::use_program() {
    if(this->resource_state != LoadableResourceState::LOADED) { return; }
    /* Only update the current program if the current program is not already
    the current program.  I'm not actually sure if this has any performance
    benefit, but I don't thin it's going to have a significant performance
    impact so I'm going to leave it in. */
    if(this->current_program != this->gl_program) {
        glUseProgram(this->gl_program);
        this->current_program = this->gl_program;
    }
}

// =======================
// == PROTECTED MEMBERS ==
// =======================

void ShaderProgram::load_() {

    this->resource_state = LoadableResourceState::LOADING;

    DEBUG("Loading shader program with fs [%s.frag]", fsname);

    // Get the shaders
    this->vshader = Shader::get_shader(engine, vsname, GL_VERTEX_SHADER, vdefines);
    this->fshader = Shader::get_shader(engine, fsname, GL_FRAGMENT_SHADER, fdefines);

    // Submit to the linking queue
    std::function job = [this](){this->link();};
    this->engine->graphics_controller->submit_graphics_task(job);

}

void ShaderProgram::unload_() {

    this->resource_state = LoadableResourceState::NOT_LOADED;

    // Submit to the unlinking queue
    std::function job = [this](){this->unlink();};
    this->engine->graphics_controller->submit_graphics_task(job);

}

uint32_t ShaderProgram::resource_id() {
    return get_shader_program_hash(this->vsname, this->vdefines,
        this->fsname, this->fdefines);
}

std::string ShaderProgram::resource_name() {
    std::string resname;
    resname += "ShaderProgram_";
    resname += this->name;
    return resname;
}