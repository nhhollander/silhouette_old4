/*!
 *  @file src/se/graphics/imageTexture.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/imageTexture.hpp"

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
#include <png.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace se::graphics;
using namespace util;

// =====================
// == PRIVATE MEMBERS ==
// =====================

#define TEXTURE_HASH_FORMAT "imagetexture:%p:%s"

ImageTexture::ImageTexture(se::Engine* engine, const char* name) : Texture(engine, name) {
    this->cache_resource(this);
}

ImageTexture::~ImageTexture() {
}

void ImageTexture::bind() {

    Texture::bind();

    /* Deleting texture data is optional, but will help conserve some memory.
    the texture re-loading process will re-read it from disk, so there's no
    need to keep a copy cached in memory after it's been bound to the disk. */
    delete this->texture_data;
    this->texture_data = nullptr;

}

// =======================
// == PROTECTED METHODS ==
// =======================

void ImageTexture::load_() {
    DEBUG("Loading texture [%s]", this->name);
    this->resource_state = LoadableResourceState::LOADING;
    
    std::string fname = util::dirs::app_data();
    fname += "/textures/";
    fname += this->name;
    fname += ".png";

    // Load the file header
    FILE* fp = fopen(fname.c_str(), "rb");
    if(fp == nullptr) {
        ERROR("[%s] Failed to open texture file [%s] [%i: %s]",
            this->name, fname.c_str(), errno, strerror(errno));
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    char png_signature[8];
    size_t header_read_count = fread(png_signature, 1, 8, fp);
    if(header_read_count != 8) {
        ERROR("[%s] Read wrong number of signature bytes (expected %i, got %i)",
            8, header_read_count);
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    if(png_sig_cmp((png_const_bytep) png_signature, 0, 8)) {
        ERROR("[%s] Not a PNG file [%s]",
            this->name, fname.c_str());
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
        ERROR("[%s] Failed to create png read structure", this->name);
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        ERROR("[%s] Failed to create png info structure", this->name);
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    if(setjmp(png_jmpbuf(png_ptr))) {
        ERROR("[%s] Encountered error during init_io", this->name);
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    this->options.dimx = png_get_image_width(png_ptr, info_ptr);
    this->options.dimy = png_get_image_height(png_ptr, info_ptr);
    //png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    //png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    //int number_of_passes = png_set_interlace_handling(png_ptr);
    // TODO: What do the above 3 variables actually mean, and why should I care?
    uint32_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    png_read_update_info(png_ptr, info_ptr);

    // This is the part where the png file actually gets read
    if(setjmp(png_jmpbuf(png_ptr))) {
        ERROR("[%s] Error during read_image", this->name);
        this->resource_state = LoadableResourceState::ERROR;
        return;
    }
    /* We're reading the image data into this multi-array structure first
    because that's the way libpng does it. */
    png_bytep* rows[this->options.dimx];
    for(int y = 0; y < this->options.dimy; y++) {
        rows[y] = new png_bytep[row_bytes];
    }
    png_read_image(png_ptr, (png_bytepp) rows);
    /* Now we convert the row data into a single continuous string of bytes so
    that it can be bound by opengl */
    this->texture_data = new char[this->options.dimy * row_bytes];
    for(int y = 0; y < this->options.dimy; y++) {
        unsigned int index = (this->options.dimy - y) - 1;
        memcpy(this->texture_data + (y * row_bytes), rows[index], row_bytes);
        delete[] rows[index];
    }

    // Submit to binding queue
    DEBUG("Loaded [%s], waiting for bind", this->name);
    std::function job = [this](){this->bind();};
    this->engine->graphics_controller->submit_graphics_task(job);

}

void ImageTexture::unload_() {
    DEBUG("Unloading [%s], waiting for unbind", this->name);
    this->resource_state = LoadableResourceState::NOT_LOADED;
    std::function job = [this](){this->unbind();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

uint32_t ImageTexture::resource_id() {
    return util::hash::ejenkins(TEXTURE_HASH_FORMAT, engine, name);
}

std::string ImageTexture::resource_name() {
    std::string resname;
    resname += "ImageTexture_";
    resname += this->name;
    return resname;
}

// ====================
// == PUBLIC METHODS ==
// ====================

ImageTexture* ImageTexture::get_texture(se::Engine* engine, const char* name) {
    uint32_t hash = util::hash::ejenkins(TEXTURE_HASH_FORMAT, engine, name);
    CacheableResource* resource = ImageTexture::find_resource(hash);
    if(resource == nullptr) {
        DEBUG("Texture [%s] not in cache :(", name);
        return new ImageTexture(engine, name);
    }
    DEBUG("Found texture [%s] in cache!", name);
    return static_cast<ImageTexture*>(resource);
}