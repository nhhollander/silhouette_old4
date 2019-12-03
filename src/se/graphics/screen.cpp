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
#include "se/graphics/texture.hpp"

#include "util/log.hpp"
#include "util/config.hpp"

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

void Screen::init() {
    DEBUG("Initializing");
    // Generate the screen geometry data
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

    this->reconfigure_textures();
    this->ready = true;
}

void Screen::deinit() {
    this->ready = false;
    glDeleteVertexArrays(1, &this->gl_screen_vert_array_id);
    glDeleteBuffers(1, &this->gl_screen_vert_buffer_id);
}

void Screen::reconfigure_textures() {
    this->primary_color_tex->options.dimx = *this->dimx;
    this->primary_color_tex->options.dimy = *this->dimy;
    this->primary_color_tex->options.mscount = *this->msaa;

    this->primary_bg_tex->options.dimx = *this->dimx;
    this->primary_bg_tex->options.dimy = *this->dimy;
    this->primary_bg_tex->options.mscount = *this->msaa;

    this->primary_depth_tex->options.dimx = *this->dimx;
    this->primary_depth_tex->options.dimy = *this->dimy;
    this->primary_depth_tex->options.mscount = *this->msaa;

    this->post_color_tex->options.dimx = *this->dimx;
    this->post_color_tex->options.dimy = *this->dimy;

    this->primarybuffer->re_init();
    this->postprocessbuffer->re_init();
}

// ===================
// == PUBLIC MBMERS ==
// ===================

Screen::Screen(se::Engine* engine) {
    // Save variables
    this->engine = engine;
    // Get shader programs
    this->output_fbid = engine->config->get_intp("internal.gl.outputfbid");
    this->screen_program = ShaderProgram::get_program(
        engine, "screen", "", "screen", "");
    this->screen_program->increment_resource_user_counter();
    this->post_process_program = ShaderProgram::get_program(
        engine, "screen", "", "screen_post", "");
    this->post_process_program->increment_resource_user_counter();
    this->engine->graphics_controller->submit_graphics_task([this](){
        this->init();
    });
    // Get pointers
    this->dimx = engine->config->get_intp("window.dimx");
    this->dimy = engine->config->get_intp("window.dimy");
    this->msaa = engine->config->get_intp("render.msaa");
    // Create textures
    this->primary_color_tex = new Texture(this->engine, "fb_primary_color_tex");
    this->primary_color_tex->options.gl_color_attachment = GL_COLOR_ATTACHMENT0;
    this->primary_color_tex->options.type = GL_TEXTURE_2D_MULTISAMPLE;
    this->primary_color_tex->options.gl_color_format = GL_RGBA8;
    this->primary_color_tex->options.gl_tex_wrap_s = GL_CLAMP_TO_EDGE;
    this->primary_color_tex->options.gl_tex_wrap_t = GL_CLAMP_TO_EDGE;

    this->primary_bg_tex = new Texture(this->engine, "fb_primary_bg_tex");
    this->primary_bg_tex->options.gl_color_attachment = GL_COLOR_ATTACHMENT1;
    this->primary_bg_tex->options.type = GL_TEXTURE_2D_MULTISAMPLE;
    this->primary_bg_tex->options.gl_color_format = GL_RGBA8;

    this->primary_depth_tex = new Texture(this->engine, "fb_primary_depth_tex");
    this->primary_depth_tex->options.gl_color_attachment = GL_DEPTH_ATTACHMENT;
    this->primary_depth_tex->options.gl_color_format = GL_DEPTH_COMPONENT24;
    this->primary_depth_tex->options.type = GL_TEXTURE_2D_MULTISAMPLE;


    this->post_color_tex = new Texture(this->engine, "fb_post_color_tex");
    this->post_color_tex->options.gl_color_attachment = GL_COLOR_ATTACHMENT0;
    this->post_color_tex->options.type = GL_TEXTURE_2D;
    this->post_color_tex->options.gl_tex_wrap_s = GL_CLAMP_TO_EDGE;
    this->post_color_tex->options.gl_tex_wrap_t = GL_CLAMP_TO_EDGE;
    // Create framebuffers
    std::vector<Texture*> primary_textures;
    primary_textures.push_back(this->primary_color_tex);
    primary_textures.push_back(this->primary_bg_tex);
    primary_textures.push_back(this->primary_depth_tex);
    this->primarybuffer = new Framebuffer(engine, primary_textures);
    std::vector<Texture*> post_textures;
    post_textures.push_back(this->post_color_tex);
    this->postprocessbuffer = new Framebuffer(engine, post_textures);

    // Configure the resize and reconfiguration handlers
    auto handler = [this](util::ConfigurationValue* a,util::Configuration* b){
        this->reconfigure_textures();};
    this->engine->config->get("window.dimx")->add_change_handler(handler);
    this->engine->config->get("window.dimy")->add_change_handler(handler);
    this->engine->config->get("render.msaa")->add_change_handler(handler);
}

Screen::~Screen() {
    this->screen_program->decrement_resource_user_counter();
    this->post_process_program->decrement_resource_user_counter();
    delete this->primarybuffer;
    delete this->postprocessbuffer;
}

void Screen::activate_framebuffer() {
    if(!this->ready) { return; }
    this->primarybuffer->use_as_target();
}

void Screen::render() {
    if(!this->ready) { return; }

    // First pass
    this->postprocessbuffer->use_as_target();
    this->screen_program->use_program();
    glUniform1i(SE_SHADER_LOC_DIMX, *this->dimx);
    glUniform1i(SE_SHADER_LOC_DIMY, *this->dimy);
    glUniform1i(SE_SHADER_LOC_MSAA_LEVEL, *this->msaa);
    //glUniform1f(SE_SHADER_LOC_CAM_NEAR, *this->cam_near);
    //glUniform1f(SE_SHADER_LOC_CAM_FAR, *this->cam_far);
    this->primary_color_tex->use_texture(GL_TEXTURE0);
    this->primary_bg_tex->use_texture(GL_TEXTURE1);
    this->primary_depth_tex->use_texture(GL_TEXTURE2);
    glBindVertexArray(this->gl_screen_vert_array_id);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Second pass
    glBindFramebuffer(GL_FRAMEBUFFER, *this->output_fbid);
    this->post_process_program->use_program();
    this->post_color_tex->use_texture(GL_TEXTURE0);
    glBindVertexArray(this->gl_screen_vert_array_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}