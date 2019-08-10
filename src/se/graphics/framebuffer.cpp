/*!
 *  @file src/se/graphics/framebuffer.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/framebuffer.hpp"

#include "se/engine.hpp"
#include "se/graphics/shader.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/config.hpp"
#include "util/debugstrings.hpp"
#include "util/log.hpp"

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void Framebuffer::re_init() {
    this->dimx = this->engine->config->get_int("window.dimx");
    this->dimy = this->engine->config->get_int("window.dimy");
    this->msaa_level = this->engine->config->get_int("render.msaa");
    // TODO: Sanity check the values
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
        this->init();
    });
}

void Framebuffer::init() {
    this->state = FramebufferState::INITIALIZING;

    DEBUG("Initializing Framebuffer");
    // Step 1 - Generate and bind texture
    DEBUG("Generating Texture");
    glGenTextures(1, &this->gl_texture_id);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->gl_texture_id);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->msaa_level,
        GL_RGBA8, this->dimx, this->dimy, true);
    // Step 2 - Generate and bind framebuffer
    DEBUG("Generating Framebuffer");
    glGenFramebuffers(1, &this->gl_framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+this->location,
        GL_TEXTURE_2D_MULTISAMPLE, this->gl_texture_id, 0);
    // Step 3 - Generate depth buffer
    DEBUG("Generating depth buffer");
    glGenRenderbuffers(1, &this->gl_depthbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, this->gl_depthbuffer_id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, this->msaa_level, GL_DEPTH24_STENCIL8, this->dimx, this->dimy);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, this->gl_depthbuffer_id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, this->gl_depthbuffer_id);
    // Step 4 - Configure draw buffers
    GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers);
    
    // TODO: Is htis required?
    glEnable(GL_MULTISAMPLE);

    GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("Failed to generate framebuffer! [%s]",
            util::string::gl_framebuffer_status_name(framebuffer_status));
        this->state = FramebufferState::ERROR;
    } else {
        this->state = FramebufferState::INITIALIZED;
    }

    DEBUG("Framebuffer initialization complete");
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

Framebuffer::Framebuffer(se::Engine* engine, unsigned int location) {
    this->engine = engine;
    this->location = location;
    this->dimx = this->engine->config->get_int("window.dimx");
    this->dimy = this->engine->config->get_int("window.dimy");
    this->msaa_level = this->engine->config->get_int("render.msaa");

    auto handler = [this](util::ConfigurationValue* a,util::Configuration* b){
        this->re_init();};
    this->engine->config->get("window.dimx")->add_change_handler(handler);
    this->engine->config->get("window.dimy")->add_change_handler(handler);
    this->engine->config->get("render.msaa")->add_change_handler(handler);

    this->engine->graphics_controller->submit_graphics_task([this](){
        this->init();
    });
}

Framebuffer::~Framebuffer() {
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
    });
}

void Framebuffer::use_as_target() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);
}

void Framebuffer::use_as_texture(unsigned int unit) {
    int unit_num = GL_TEXTURE0 - unit;
    glUniform1i(SE_SHADER_LOC_TEX_0 + unit_num, unit_num);
    glUniform1i(SE_SHADER_LOC_DIMX, this->dimx);
    glUniform1i(SE_SHADER_LOC_DIMY, this->dimy);
    glUniform1i(SE_SHADER_LOC_MSAA_LEVEL, this->msaa_level);
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->gl_texture_id);
}