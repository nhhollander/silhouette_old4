/*!
 *  @file src/se/graphics/framebuffer.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/framebuffer.hpp"

#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void Framebuffer::window_resize_handler() {
    this->dimx = this->engine->config->get_int("window.dimx");
    this->dimy = this->engine->config->get_int("window.dimy");
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
        this->init();
    });
}

void Framebuffer::init() {
    this->state = FramebufferState::INITIALIZING;
    // Step 1 - Generate and bind the framebuffer
    glGenFramebuffers(1, &this->gl_framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);
    // Step 2 - Generate and bind the texture(s)
    glGenTextures(1, &this->gl_texture_id);
    glBindTexture(GL_TEXTURE_2D, this->gl_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->dimx, this->dimy, 0, GL_RGB,
        GL_UNSIGNED_BYTE, 0); // Last '0' generates a blank image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Step 2 - Generate the depth buffer
    glGenRenderbuffers(1, &this->gl_depthbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, this->gl_depthbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
        this->dimx, this->dimy);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, this->gl_depthbuffer_id);
    // Step 3 - Configure the framebuffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + this->location,
        this->gl_texture_id, 0);
    GLenum buffers[1] = {GL_COLOR_ATTACHMENT0 + this->location};
    glDrawBuffers(1, buffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER), GL_FRAMEBUFFER_COMPLETE) {
        ERROR("Failed to generate framebuffer!");
        this->state = FramebufferState::ERROR;
    } else {
        this->state = FramebufferState::INITIALIZED;
    }
}

void Framebuffer::deinit() {
    this->state = FramebufferState::DE_INITIALIZING;

    glDeleteFramebuffers(1, &this->gl_framebuffer_id);
    glDeleteTextures(1, &this->gl_texture_id);
    glDeleteRenderbuffers(1, &this->gl_depthbuffer_id);

    this->state = FramebufferState::NOT_INITIALIZED;
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

Framebuffer::Framebuffer(se::Engine* engine, int location) {
    this->engine = engine;
    this->location = location;
    this->dimx = this->engine->config->get_int("window.dimx");
    this->dimy = this->engine->config->get_int("window.dimy");
    this->engine->config->get("window.dimx")->add_change_handler(
        [this](util::ConfigurationValue* a,util::Configuration* b){
            this->window_resize_handler();
        });
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->init();
    });
}

Framebuffer::~Framebuffer() {
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
    });
}

void Framebuffer::use() {

}
