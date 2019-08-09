/*!
 *  @file src/se/graphics/screen.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/screen.hpp"

#include "se/engine.hpp"
#include "se/graphics/framebuffer.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/shaderProgram.hpp"
#include "se/graphics/shader.hpp"

#include "util/log.hpp"

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void Screen::init() {
    DEBUG("Initializing");
    static const GLfloat screen_vertex_data[] = {
        -1.0, -1.0, 0.0,
         1.0, -1.0, 0.0,
        -1.0,  1.0, 0.0,
        -1.0,  1.0, 0.0,
         1.0, -1.0, 0.0,
         1.0,  1.0, 0.0, 
    };
    glGenVertexArrays(1, &this->gl_screen_vert_array_id);
    glBindVertexArray(this->gl_screen_vert_array_id);
    glGenBuffers(1, &this->gl_screen_vert_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->gl_screen_vert_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_data), screen_vertex_data, GL_STATIC_DRAW);
    glVertexAttribPointer(SE_SHADER_LOC_IN_VERT, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SE_SHADER_LOC_IN_VERT);
    this->ready = true;
}

void Screen::deinit() {
    this->ready = false;
    glDeleteVertexArrays(1, &this->gl_screen_vert_array_id);
    glDeleteBuffers(1, &this->gl_screen_vert_buffer_id);
}

// ===================
// == PUBLIC MBMERS ==
// ===================

Screen::Screen(se::Engine* engine) {
    this->engine = engine;
    this->screen_program = ShaderProgram::get_program(
        engine, "screen", "", "screen", "");
    this->screen_program->increment_active_users();
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->init();
    });
    this->framebuffer = new Framebuffer(engine, 0);
}

Screen::~Screen() {
    this->screen_program->decrement_active_users();
    delete this->framebuffer;
}

void Screen::activate_framebuffer() {
    if(!this->ready) { return; }
    this->framebuffer->use_as_target();
}

void Screen::render() {
    if(!this->ready) { return; }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 0 is the display device
    this->screen_program->use_program();
    glBindVertexArray(this->gl_screen_vert_array_id);
    glDisable(GL_DEPTH_TEST);
    this->framebuffer->use_as_texture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}