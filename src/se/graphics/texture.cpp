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

#define TEXTURE_HASH_FORMAT "texture:%p:%s"

// =====================
// == PRIVATE METHODS ==
// =====================

Texture::Texture(se::Engine* engine, const char* name) : 
    GraphicsResource(util::hash::ejenkins(TEXTURE_HASH_FORMAT, engine, name)) {
    this->engine = engine;
    this->name = strdup(name);
}

// =======================
// == PROTECTED METHODS ==
// =======================

Texture::Texture(se::Engine* engine, const char* name, uint32_t hash) : 
    GraphicsResource(hash) {
    this->engine = engine;
    this->name = strdup(name);
}

Texture::~Texture() {
    free((void*)this->name);
}

void Texture::bind() {
    /* Is clearing the error queue actually necessary?  I'm not sure. */
    while(glGetError() != GL_NO_ERROR) {}

    glGenTextures(1, &this->gl_texture);
    glBindTexture(GL_TEXTURE_2D, this->gl_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0,
        GL_RGB, GL_UNSIGNED_BYTE, 0);
    // TODO: Configure Sampling
    // TODO: Configure Wrapping
    // TODO: Configure MipMapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        ERROR("[%s] Failed to bind texture [%s: %s]", this->name,
            util::string::gl_error_name(err), util::string::gl_error_desc(err));
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    
    DEBUG("Texture [%s] bound successfuly as!", this->name);
    this->resource_state = GraphicsResourceState::LOADED;
}

void Texture::unbind() {
    glDeleteTextures(1, &this->gl_texture);
    this->gl_texture = 0;
    DEBUG("Texture [%s] unbound", this->name);
}

void Texture::load_() {

}

void Texture::unload_() {
    DEBUG("Unloading [%s], waiting for unbind", this->name);
    this->resource_state = GraphicsResourceState::NOT_LOADED;
    std::function job = [this](){this->unbind();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

// ====================
// == PUBLIC METHODS ==
// ====================

Texture* Texture::get_texture(se::Engine* engine, const char* name, bool virt) {
    uint32_t hash = util::hash::ejenkins(TEXTURE_HASH_FORMAT, engine, name, virt);
    GraphicsResource* resource = Texture::get_resource(hash);
    if(resource == nullptr) {
        DEBUG("Texture [%s] not in cache :(", name);
        return new Texture(engine, name, virt);
    }
    DEBUG("Found texture [%s] in cache!", name);
    return static_cast<Texture*>(resource);
}

void Texture::use_texture(unsigned int tex_unit) {
    if(this->resource_state != GraphicsResourceState::LOADED) { return; }
    int unit_num = GL_TEXTURE0 - tex_unit;
    glUniform1i(SE_SHADER_LOC_TEX_0 + unit_num, unit_num);
    glActiveTexture(tex_unit);
    glBindTexture(GL_TEXTURE_2D, this->gl_texture);
}