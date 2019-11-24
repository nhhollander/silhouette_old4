/*!
 *  @file src/se/graphics/textTexture.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/textTexture.hpp"

#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/log.hpp"
#include "util/debugstrings.hpp"
#include "util/dirs.hpp"

#include <codecvt>
#include <locale>

using namespace se::graphics;
using namespace util;

// =====================
// == PRIVATE METHODS ==
// =====================

FT_Vector TextTexture::render_char(char32_t glyph, int x, int y) {
    DEBUG("Rendering character [%c] at [%i,%i]", glyph, x, y);
    FT_Error error = FT_Load_Char(this->font_face, glyph, FT_LOAD_RENDER);
    if(error) {
        ERROR("Failed to load glyph [%c] [%s]", glyph,
            util::string::ft2_error_name(error));
        // TODO: More error detail here
        return {0,0};
    }
    // Calculate the glyph origin
    int g_orig_x = x + this->font_face->glyph->bitmap_left;
    int g_orig_y = y - this->font_face->glyph->bitmap_top;
    // Render the glyph to the buffer
    for(unsigned int g_y = 0; g_y < this->font_face->glyph->bitmap.rows; g_y++) {
        for(unsigned int g_x = 0; g_x < this->font_face->glyph->bitmap.width; g_x++) {
            // Sanity check the foordinate
            int buf_x = g_orig_x + g_x;
            int buf_y = g_orig_y + g_y;
            if(buf_x < 0 || buf_x >= this->options.dimx || buf_y < 0 || buf_y >= this->options.dimy) {
                DEBUG("Character out of bounds [%i,%i]", buf_x, buf_y);
                continue;
            }
            // Get offsets and ocpy texture data
            int glyph_offset = g_x + (g_y * this->font_face->glyph->bitmap.width);
            int buffer_offset = buf_x + (buf_y * this->options.dimx);
            this->texture_data[buffer_offset] = this->font_face->glyph->bitmap.buffer[glyph_offset];
            //this->tex_buffer[buffer_offset] = 0xff;
        }
    }
    return this->font_face->glyph->advance;
}

// ====================
// == PUBLIC METHODS ==
// ====================

TextTexture::TextTexture(se::Engine* engine, const char* name) : Texture(engine, name) {    
    auto err = FT_Init_FreeType(&this->ft);
    if(err) {
        ERROR("Failed to initialize FreeType library [%s]",
            util::string::ft2_error_name(err));
        this->init_success = false;
        return;
    }

    // Default font configuration
    this->set_font(util::dirs::app_data() + "/fonts/roboto.ttf");
    this->set_size(45);
    this->set_texturesize(256,256);

    this->options.gl_color_format = GL_RED;
    this->options.gl_data_format = GL_RED;
}

TextTexture::~TextTexture() {}

bool TextTexture::set_font(std::string font) {
    FT_Error error = FT_New_Face(this->ft, font.c_str(), 0, &this->font_face);
    if(error) {
        ERROR("Failed to load font [%s] [%s]", font.c_str(),
            util::string::ft2_error_name(error));
        return false;
    }
    DEBUG("Set font to [%s]", font.c_str());
    return true;
}

bool TextTexture::set_size(int height) {
    FT_Error error = FT_Set_Pixel_Sizes(this->font_face, 0, height);
    if(error) {
        ERROR("Failed to set font size to [%i] [%s]", height,
            util::string::ft2_error_name(error));
        return false;
    }
    DEBUG("Set font size to [%i]", height);
    return true;
}

void TextTexture::set_texturesize(int dimx, int dimy) {
    if(dimx < 0 || dimy < 0) {
        ERROR("Invalid texture dimensions [%ix%i]", dimx, dimy);
        return;
    }
    this->options.dimx = dimx;
    this->options.dimy = dimy;
}

void TextTexture::update_text(std::string text) {
    // Reset the buffer
    delete[] this->texture_data;
    int length = this->options.dimx * this->options.dimy;
    this->texture_data = new char[length];
    for(int i = 0; i < length; i++) {
        this->texture_data[i] = 0x00;
    }
    // Current position
    int x = this->origin_x;
    int y = this->origin_y;
    // Convert string to utf32 for processing
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::u32string utf32 = converter.from_bytes(text);
    // Render the string
    for(char32_t c : utf32) {
        if(c == '\n') {
            // TODO: Temporary hack
            x = this->origin_x;
            y += 45;
            continue;
        }
        FT_Vector advance = this->render_char(c, x, y);
        x += (advance.x / 64);
        y += (advance.y / 64);
    }
    // Bind the texture
    std::function job = [this](){
        this->unbind();
        this->bind();
    };
    this->engine->graphics_controller->submit_graphics_task(job);
}