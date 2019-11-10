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
    // TODO: Sanity check the values
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->deinit();
        this->init();
    });
}

void Framebuffer::init() {
    this->state = FramebufferState::INITIALIZING;

    if(this->type == FramebufferType::MULTISAMPLE) {

        // Generate and bind framebuffer
        glGenFramebuffers(1, &this->gl_framebuffer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);

        // Utilized buffers
        std::vector<GLenum> enabled_buffers;

        // Generate and bind primary texture
        if(this->base_texture_enabled) {
            glGenTextures(1, &this->gl_texture_id);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->gl_texture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, *this->msaa_level,
                GL_RGBA8, *this->dimx, *this->dimy, true);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D_MULTISAMPLE, this->gl_texture_id, 0);
            enabled_buffers.push_back(GL_COLOR_ATTACHMENT0);
        }
        // Generate and bind background texture
        if(this->bg_texture_enabled) {
            glGenTextures(1, &this->gl_bg_texture_id);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->gl_bg_texture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, *this->msaa_level,
                GL_RGBA8, *this->dimx, *this->dimy, true);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                GL_TEXTURE_2D_MULTISAMPLE, this->gl_bg_texture_id, 0);
            enabled_buffers.push_back(GL_COLOR_ATTACHMENT1);
        }
        // Generate and bind depth texture
        if(this->depth_texture_enabled) {
            glGenTextures(1, &this->gl_depth_texture_id);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->gl_depth_texture_id);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, *this->msaa_level,
                GL_DEPTH_COMPONENT24, *this->dimx, *this->dimy, true);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D_MULTISAMPLE, this->gl_depth_texture_id, 0);
        }

        if(enabled_buffers.size() > 0) {
            glDrawBuffers(enabled_buffers.size(), &enabled_buffers[0]);
        } else {
            WARN("No buffers enabled, framebuffer is broken!");
        }
        
        // TODO: Is htis required?
        glEnable(GL_MULTISAMPLE);

    } else {

        // TODO: non-multisampled depth buffer

        // Generate and bind framebuffer
        glGenFramebuffers(1, &this->gl_framebuffer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);

        // Utilized buffers
        std::vector<GLenum> enabled_buffers;

        // Generate and bind primary texture
        if(this->base_texture_enabled) {
            glGenTextures(1, &this->gl_texture_id);
            glBindTexture(GL_TEXTURE_2D, this->gl_texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *this->dimx, *this->dimy, 0,
                GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                this->texture_wrapping_mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                this->texture_wrapping_mode);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D, this->gl_texture_id, 0);
            enabled_buffers.push_back(GL_COLOR_ATTACHMENT0);
        }
        // Generate and bind background texture
        if(this->bg_texture_enabled) {
            glGenTextures(1, &this->gl_bg_texture_id);
            glBindTexture(GL_TEXTURE_2D, this->gl_bg_texture_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *this->dimx, *this->dimy, 0,
                GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                this->texture_wrapping_mode);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                this->texture_wrapping_mode);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                GL_TEXTURE_2D, this->gl_bg_texture_id, 0);
            enabled_buffers.push_back(GL_COLOR_ATTACHMENT1);
        }
        // Generate and bind depth texture
        if(this->depth_texture_enabled) {
            // Oh heck this is broken
            //glGenTextures(1, &this->gl_depth_texture_id);
            //glBindTexture(GL_TEXTURE_2D, this->gl_depth_texture_id);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *this->dimx, *this->dimy, 0,
            //    GL_RGB, GL_UNSIGNED_BYTE, NULL);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            //    GL_TEXTURE_2D, this->gl_depth_texture_id, 0);
        }
        
        if(enabled_buffers.size() > 0) {
            glDrawBuffers(enabled_buffers.size(), &enabled_buffers[0]);
        } else {
            WARN("No buffers enabled, framebuffer is broken!");
        }

    }

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
    glDeleteTextures(1, &this->gl_texture_id);
    glDeleteTextures(1, &this->gl_bg_texture_id);
    glDeleteTextures(1, &this->gl_depth_texture_id);
    glDeleteRenderbuffers(1, &this->gl_depthbuffer_id);

    this->state = FramebufferState::NOT_INITIALIZED;
}

// ====================
// == PUBLIC MEMBERS ==
// ====================

Framebuffer::Framebuffer(se::Engine* engine, FramebufferType type,
    bool base_texture_enabled, bool bg_texture_enabled,
    bool depth_texture_enabled, GLenum texture_wrapping_mode) {
    this->engine = engine;
    this->type = type;
    this->base_texture_enabled = base_texture_enabled;
    this->bg_texture_enabled = bg_texture_enabled;
    this->depth_texture_enabled = depth_texture_enabled;
    this->texture_wrapping_mode = texture_wrapping_mode;
    this->dimx = this->engine->config->get_intp("window.dimx");
    this->dimy = this->engine->config->get_intp("window.dimy");
    this->msaa_level = this->engine->config->get_intp("render.msaa");
    this->cam_near = this->engine->config->get_floatp("render.cam_near");
    this->cam_far = this->engine->config->get_floatp("render.cam_far");

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
    // TODO: Wait for de-init somehow
}

void Framebuffer::use_as_target() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->gl_framebuffer_id);
}

void Framebuffer::use_as_texture() {
    auto tex_type = this->type == FramebufferType::MULTISAMPLE
        ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    glUniform1i(SE_SHADER_LOC_TEX_SCR_COLOR, 0);
    glUniform1i(SE_SHADER_LOC_TEX_SCR_BG, 1);
    glUniform1i(SE_SHADER_LOC_TEX_SCR_DEPTH, 2);
    glUniform1i(SE_SHADER_LOC_DIMX, *this->dimx);
    glUniform1i(SE_SHADER_LOC_DIMY, *this->dimy);
    glUniform1i(SE_SHADER_LOC_MSAA_LEVEL, *this->msaa_level);
    glUniform1f(SE_SHADER_LOC_CAM_NEAR, *this->cam_near);
    glUniform1f(SE_SHADER_LOC_CAM_FAR, *this->cam_far);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(tex_type, this->gl_texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(tex_type, this->gl_bg_texture_id);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(tex_type, this->gl_depth_texture_id);
}