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
#include <png.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

#define TEXTURE_HASH_FORMAT "texture:%p:%s"

Texture::Texture(se::Engine* engine, const char* name) : 
    GraphicsResource(util::hash::ejenkins(TEXTURE_HASH_FORMAT, engine, name)) {
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
        GL_RGB, GL_UNSIGNED_BYTE, this->raw_texture);
    // TODO: Configure Sampling
    // TODO: Configure Wrapping
    // TODO: Configure MipMapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    delete[] this->raw_texture;

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

// =======================
// == PROTECTED METHODS ==
// =======================

void Texture::load_() {
    DEBUG("Loading texture [%s]", this->name);
    this->resource_state = GraphicsResourceState::LOADING;
    
    std::string fname = util::dirs::app_data();
    fname += "/textures/";
    fname += this->name;
    fname += ".png";

    // Load the file header
    FILE* fp = fopen(fname.c_str(), "rb");
    if(fp == nullptr) {
        ERROR("[%s] Failed to open texture file [%s] [%i: %s]",
            this->name, fname.c_str(), errno, strerror(errno));
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    char png_signature[8];
    size_t header_read_count = fread(png_signature, 1, 8, fp);
    if(header_read_count != 8) {
        ERROR("[%s] Read wrong number of signature bytes (expected %i, got %i)",
            8, header_read_count);
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    if(png_sig_cmp((png_const_bytep) png_signature, 0, 8)) {
        ERROR("[%s] Not a PNG file [%s]",
            this->name, fname.c_str());
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
        ERROR("[%s] Failed to create png read structure", this->name);
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        ERROR("[%s] Failed to create png info structure", this->name);
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    if(setjmp(png_jmpbuf(png_ptr))) {
        ERROR("[%s] Encountered error during init_io", this->name);
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    this->width = png_get_image_width(png_ptr, info_ptr);
    this->height = png_get_image_height(png_ptr, info_ptr);
    //png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    //png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    //int number_of_passes = png_set_interlace_handling(png_ptr);
    // TODO: What do the above 3 variables actually mean, and why should I care?
    uint32_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    png_read_update_info(png_ptr, info_ptr);

    // This is the part where the png file actually gets read
    if(setjmp(png_jmpbuf(png_ptr))) {
        ERROR("[%s] Error during read_image", this->name);
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }
    /* We're reading the image data into this multi-array structure first
    because that's the way libpng does it. */
    png_bytep* rows[this->height];
    for(int y = 0; y < this->height; y++) {
        rows[y] = new png_bytep[row_bytes];
    }
    png_read_image(png_ptr, (png_bytepp) rows);
    /* Now we convert the row data into a single continuous string of bytes so
    that it can be bound by opengl */
    this->raw_texture = new char[this->height * row_bytes];
    for(int y = 0; y < this->height; y++) {
        unsigned int index = (this->height - y) - 1;
        memcpy(this->raw_texture + (y * row_bytes), rows[index], row_bytes);
        delete[] rows[index];
    }

    // Submit to binding queue
    DEBUG("Loaded [%s], waiting for bind", this->name);
    std::function job = [this](){this->bind();};
    this->engine->graphics_controller->submit_graphics_task(job);

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

Texture* Texture::get_texture(se::Engine* engine, const char* name) {
    uint32_t hash = util::hash::ejenkins(TEXTURE_HASH_FORMAT, engine, name);
    GraphicsResource* resource = Texture::get_resource(hash);
    if(resource == nullptr) {
        DEBUG("Texture [%s] not in cache :(", name);
        return new Texture(engine, name);
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