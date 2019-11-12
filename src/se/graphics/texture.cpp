/*!
 *  @file src/se/graphics/texture.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/texture.hpp"

#include "se/engine.hpp"
#include "se/graphics/shader.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/dirs.hpp"
#include "util/hash.hpp"
#include "util/log.hpp"
#include "util/debugstrings.hpp"

#include <string.h>
#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace se::graphics;
using namespace util;

#define TEXTURE_HASH_FORMAT "texture:%p:%s"

// =======================
// == PROTECTED METHODS ==
// =======================

Texture::~Texture() {
    free((void*)this->name);
}

void Texture::bind() {
    /* Is clearing the error queue actually necessary?  I'm not sure. */
    while(glGetError() != GL_NO_ERROR) {}

    if(this->options.type != GL_TEXTURE_2D && this->options.type != GL_TEXTURE_2D_MULTISAMPLE) {
        ERROR("Unsupported texture type [%s]", util::string::gl_type_name(this->options.type));
        return;
    }

    glGenTextures(1, &this->gl_texture);
    glBindTexture(this->options.type, this->gl_texture);
    
    // TODO: Add additional texture parameter configuration options
    glTexParameteri(this->options.type, GL_TEXTURE_MAG_FILTER, this->options.gl_mag_filter);
    glTexParameteri(this->options.type, GL_TEXTURE_MIN_FILTER, this->options.gl_min_filter);
    glTexParameteri(this->options.type, GL_TEXTURE_WRAP_S,     this->options.gl_tex_wrap_s);
    glTexParameteri(this->options.type, GL_TEXTURE_WRAP_T,     this->options.gl_tex_wrap_t);

    if(this->options.type == GL_TEXTURE_2D) {
        glTexImage2D(GL_TEXTURE_2D, 0, this->options.gl_color_format, this->options.dimx, 
            this->options.dimy, 0, this->options.gl_data_format,
            GL_UNSIGNED_BYTE, this->texture_data);
    } else if(this->options.type == GL_TEXTURE_2D_MULTISAMPLE) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->options.mscount,
            this->options.gl_color_format, this->options.dimx,
            this->options.dimy, true);
    }

    if(this->options.gl_color_attachment != 0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            this->options.gl_color_attachment, this->options.type,
            this->gl_texture, 0);
    }

    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        ERROR("[%s] Failed to bind texture [%s: %s]", this->name,
            util::string::gl_error_name(err), util::string::gl_error_desc(err));
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    
    DEBUG("Texture [%s] bound successfuly as [%i]!", this->name, this->gl_texture);
    this->resource_state = LoadableResourceState::LOADED;
}

void Texture::unbind() {
    glDeleteTextures(1, &this->gl_texture);
    this->gl_texture = 0;
    DEBUG("Texture [%s] unbound", this->name);
}

void Texture::load_() {
    DEBUG("Loaded [%s], waiting for bind", this->name);
    std::function job = [this](){this->bind();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

void Texture::unload_() {
    DEBUG("Unloading [%s], waiting for unbind", this->name);
    this->resource_state = LoadableResourceState::NOT_LOADED;
    std::function job = [this](){this->unbind();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

// ====================
// == PUBLIC METHODS ==
// ====================

Texture::Texture(se::Engine* engine, const char* name) {
    this->engine = engine;
    this->name = strdup(name);
}

void Texture::use_texture(unsigned int tex_unit) {
    if(this->resource_state != LoadableResourceState::LOADED) { return; }
    int unit_num = GL_TEXTURE0 - tex_unit;
    glUniform1i(SE_SHADER_LOC_TEX_0 + unit_num, unit_num);
    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, this->gl_texture);
}