/*!
 *  @file src/se/graphics/shader.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/shader.hpp"

#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/log.hpp"
#include "util/hash.hpp"
#include "util/dirs.hpp"
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

std::map<uint32_t, Shader*> Shader::cache;

Shader* Shader::get_shader(se::Engine* engine, const char* name, GLuint type) {
    uint32_t hash = util::hash::ejenkins("%p:%s:%u", engine, name, type);

    // Check the cache
    auto check = Shader::cache.find(hash);
    if(check != Shader::cache.end()) {
        DEBUG("Found shader [%s] in cache", check->second->name);
        return check->second;
    }

    // Create a new shader
    Shader* shader = new Shader(engine, name, type);
    Shader::cache.insert(std::pair(hash, shader));
    return shader;
}

// =====================
// == PRIVATE METHODS ==
// =====================

Shader::Shader(se::Engine* engine, const char* name, GLuint type) {
    this->engine = engine;
    this->name = strdup(name);
    this->type = type;

    // Get the shader name
    this->name = std::string();
    this->name += name;
    if(this->type == GL_VERTEX_SHADER) {
        this->name += ".vert";
    } else if(this->type == GL_FRAGMENT_SHADER) {
        this->name += ".frag";
    } else {
        ERROR("[%s] Invalid or unsupported shader type [%u: %s]",
            name, type, util::string::gl_type_name(type));
        return;
    }

    // Load the shader source code from disk
    std::string fname = util::dirs::app_data() + "/shaders/" + this->name;
    FILE* fp = fopen(fname.c_str(), "rb");
    if(fp == nullptr) {
        ERROR("[%s] Failed to open source file [%s] [%i: %s]",
            this->name.c_str(), fname.c_str(), errno, strerror(errno));
        this->state = ShaderState::ERROR;
        return;
    }
    if(fseek(fp, 0, SEEK_END) != 0) {
        ERROR("[%s] Failed to locate end of source file [%s] [%i: %s]",
            this->name.c_str(), fname.c_str(), errno, strerror(errno));
        this->state = ShaderState::ERROR;
        return;
    }
    size_t filesize = ftello(fp);
    if(filesize < 0) {
        ERROR("[%s] Failed to locate end of source file (tell) [%s] [%i: %s]",
            this->name.c_str(), fname.c_str(), errno, strerror(errno));
        this->state = ShaderState::ERROR;
        return;
    }
    this->source_code = new char[filesize + 1];
    this->source_code[filesize] = '\0';
    if(fseek(fp, 0, SEEK_SET) != 0) {
        ERROR("[%s] Failed to return to beginning of source file [%s] [%i: %s]",
            this->name.c_str(), fname.c_str(), errno, strerror(errno));
        this->state = ShaderState::ERROR;
        return;
    }
    size_t read_count = fread(this->source_code, 1, filesize, fp);
    DEBUG("[%s] Loaded [%u] bytes of shader data", this->name.c_str(), read_count);
    if(read_count < filesize) {
        WARN("[%s] Read fewer bytes than expected from [%s] (expected %i, got %i)",
            this->name.c_str(), fname.c_str(), filesize, read_count);
    }
    if(fclose(fp) != 0) {
        WARN("[%s] Failed to close file handle after reading source file [%s] [%i: %s]",
            this->name.c_str(), fname.c_str(), errno, strerror(errno));
    }

    // Submit to the compilation queue
    std::function job = [this](){this->compile();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

Shader::~Shader() {
    DEBUG("Deleting shader [%p:%s]", this->engine, this->name);
    if(glIsShader(this->gl_shader) == GL_FALSE) {
        WARN("[%p:%s] No shader to delete!", this->engine, this->name);
    } else {
        glDeleteShader(this->gl_shader);
    }
}

void Shader::compile() {

    auto start_time = std::chrono::system_clock::now();

    DEBUG("Compiling Shader [%s]", this->name.c_str());

    // Create a new OpenGL shader object
    this->gl_shader = glCreateShader(this->type);
    if(this->gl_shader == 0) {
        GLenum error = glGetError();
        ERROR("[%s] Failed to create new shader [%s: %s]",
            this->name.c_str(),
            util::string::gl_error_name(error),
            util::string::gl_error_desc(error));
        this->state = ShaderState::ERROR;
        return;
    }

    // Load the source code into the object
    if(this->source_code == nullptr) {
        ERROR("[%s] Shader source code is null!",
            this->name.c_str());
        this->state = ShaderState::ERROR;
        return;
    }
    const GLchar* sources[] = {
        this->source_code
    };
    glShaderSource(this->gl_shader, 1, sources, NULL);
    delete[] this->source_code; // Source code is copied to gl

    // Compile the object and check for errors
    glCompileShader(this->gl_shader);
    GLint compiled = GL_FALSE;
    glGetShaderiv(this->gl_shader, GL_COMPILE_STATUS, &compiled);
    if(compiled != GL_TRUE) {
        // Get and print the entire compilation log
        int length;
        int max_length;
        glGetShaderiv(this->gl_shader, GL_INFO_LOG_LENGTH, &max_length);
        char* log = new char[max_length];
        glGetShaderInfoLog(this->gl_shader, max_length, &length, log);
        ERROR("[%s] Failed to compile shader:\n%s", this->name.c_str(), log);
        delete[] log;
        this->state = ShaderState::ERROR;
        return;
    }

    // Success!
    this->state = ShaderState::READY;

    auto end_time = std::chrono::system_clock::now();
    auto duration = end_time - start_time;
    uint64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    float milliseconds = time / 1000000;
    DEBUG("[%s] Shader compilation took %.3fms.",
        this->name.c_str(), milliseconds);

}

// ====================
// == PUBLIC METHODS ==
// ====================

std::string Shader::get_name() {
    return this->name;
}

GLuint Shader::get_type() {
    return this->type;
}

ShaderState Shader::get_state() {
    return this->state;
}

ShaderState Shader::wait_for_loading() {
    while(this->state == ShaderState::LOADING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return this->state;
}

GLuint Shader::get_gl_id() {
    return this->gl_shader;
}