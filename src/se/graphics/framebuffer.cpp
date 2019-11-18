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
#include "se/graphics/shader.hpp"
#include "se/graphics/texture.hpp"

#include "util/config.hpp"
#include "util/debugstrings.hpp"
#include "util/log.hpp"

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void Framebuffer::re_init() {
    // TODO: Sanity check the values
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
        this->init();
    });
}

void Framebuffer::init() {

    if(this->textures.size() <= 0) {
        this->state = FramebufferState::ERROR;
        ERROR("Attemted to initialize framebuffer with zero textures");
        return;
    }

    this->state = FramebufferState::INITIALIZING;

    // Generate and bind framebuffer
    glGenFramebuffers(1, &this->gl_framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);

    std::vector<GLenum> color_attachments;

    // Attach textures
    for(auto texture : this->textures) {
        // Verify the attachment
        GLenum attachment = texture->options.gl_color_attachment;
        if(attachment == 0) {
            WARN("Missing GL color attachment for texture!");
            continue;
        }
        texture->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            texture->options.gl_color_attachment,
            texture->options.type, texture->get_texture_id(), 0);
        if(attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT15) {
            color_attachments.push_back(attachment);
        }
    }

    glDrawBuffers(color_attachments.size(), &color_attachments[0]);

    GLenum framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        ERROR("Failed to generate framebuffer! [%s]",
            util::string::gl_framebuffer_status_name(framebuffer_status));
        this->state = FramebufferState::ERROR;
    } else {
        this->state = FramebufferState::INITIALIZED;
    }

    DEBUG("Framebuffer has been generated as [%i]", this->gl_framebuffer_id);

}

void Framebuffer::deinit() {
    this->state = FramebufferState::DE_INITIALIZING;

    glDeleteFramebuffers(1, &this->gl_framebuffer_id);

    for(auto texture : this->textures) {
        texture->unbind();
    }

    this->state = FramebufferState::NOT_INITIALIZED;
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

Framebuffer::Framebuffer(se::Engine* engine, std::vector<Texture*> textures) {
    this->engine = engine;
    this->textures = textures;

    this->engine->graphics_controller->submit_graphics_task([this](){
        this->init();
    });
}

Framebuffer::~Framebuffer() {
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
    });
    // TODO: Wait for de-init somehow
}

void Framebuffer::use_as_target() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);
}