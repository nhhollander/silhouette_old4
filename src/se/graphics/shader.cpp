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

#include "util/config.hpp"
#include "util/dirs.hpp"
#include "util/debugstrings.hpp"
#include "util/hash.hpp"
#include "util/log.hpp"

#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace se::graphics;

// ============================
// == DEFAULT SHADER DEFINES ==
// ============================

#define GLSL_DEF_STR_B(n) #n
#define GLSL_DEF_STR_A(n) GLSL_DEF_STR_B(n)
#define GLSL_DEF(n) "#define " #n " " GLSL_DEF_STR_A(SE_SHADER_##n) "\n"

/*! Default Shader Defines.  Generated from values provided in shader.hpp */
const char default_shader_defines[] =
    "#extension GL_ARB_enhanced_layouts : require\n"
    GLSL_DEF(LOC_IN_MVP)
    GLSL_DEF(LOC_IN_MODEL_MAT)
    GLSL_DEF(LOC_IN_VERT)
    GLSL_DEF(LOC_IN_UV)
    GLSL_DEF(LOC_IN_NORM)
    GLSL_DEF(LOC_CAM_NEAR)
    GLSL_DEF(LOC_CAM_FAR)
    GLSL_DEF(LOC_DIMX)
    GLSL_DEF(LOC_DIMY)
    GLSL_DEF(LOC_MSAA_LEVEL)
    GLSL_DEF(LOC_TEX_SCR_COLOR)
    GLSL_DEF(LOC_TEX_SCR_DEPTH)
    GLSL_DEF(LOC_TEX_SCR_BG)
    GLSL_DEF(LOC_TEX_0)
    "#define LOC_TEX_(n) LOC_TEX_0 + n\n"

    GLSL_DEF(LOC_OUT_COLOR)
    GLSL_DEF(LOC_OUT_BG)
    GLSL_DEF(LOC_OUT_DEPTH)
;

// ====================
// == STATIC METHODS ==
// ====================

const char* se::graphics::shader_state_name(ShaderState state) {
    switch(state) {
        case ShaderState::LOADING: return "LOADING";
        case ShaderState::READY: return "READY";
        case ShaderState::ERROR: return "ERROR";
        default: return "<invalid ShaderState>";
    }
}

std::map<uint32_t, Shader*> Shader::cache;

Shader* Shader::get_shader(se::Engine* engine, const char* name, GLuint type, const char* defines) {
    /* Hash for defines is calculated separately because there is a somewhat 
    arbitrary size limit for the ejenkins function, and it's possible that the
    definitions will overwhelm that limit, potentially poisoning the result. */
    uint32_t defines_hash = util::hash::jenkins(defines, strlen(defines));
    uint32_t hash = util::hash::ejenkins("%p:%s:%u:%u", engine, name, type, defines_hash);

    // Check the cache
    auto check = Shader::cache.find(hash);
    if(check != Shader::cache.end()) {
        DEBUG("Found shader [%s] in cache", check->second->name);
        return check->second;
    }

    // Create a new shader
    Shader* shader = new Shader(engine, name, type, defines);
    Shader::cache.insert(std::pair(hash, shader));
    return shader;
}

// =====================
// == PRIVATE METHODS ==
// =====================

Shader::Shader(se::Engine* engine, const char* name, GLuint type, const char* defines) {
    this->engine = engine;
    this->type = type;
    this->defines = strdup(defines);

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

    // Determine the version string
    int gl_major = this->engine->config->get_int("render.gl.major");
    int gl_minor = this->engine->config->get_int("render.gl.minor");
    this->vstring = new char[1024]; // Extra large for safety
    if(gl_major == 2 && gl_minor == 0) {
        strcpy(this->vstring, "#version 110\n");
    } else if(gl_major == 2 && gl_minor == 1) {
        strcpy(this->vstring, "#version 120\n");
    } else if(gl_major == 3 && gl_minor == 0) {
        strcpy(this->vstring, "#version 130\n");
    } else if(gl_major == 3 && gl_minor == 1) {
        strcpy(this->vstring, "#version 140\n");
    } else if(gl_major == 3 && gl_minor == 2) {
        strcpy(this->vstring, "#version 150\n");
    } else {
        snprintf(this->vstring, 1024, "#version %i%i\n",
            gl_major, (gl_minor * 10));
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
    free((void*) this->defines);
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
        this->vstring, // Generated #version string
        default_shader_defines, // Defaults and globals
        this->defines, // User supplied defines
        this->source_code // Loaded source code
    };
    glShaderSource(this->gl_shader, 4, sources, NULL);
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