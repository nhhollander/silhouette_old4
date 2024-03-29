/*!
 *  @file src/se/entity/skybox.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity/skybox.hpp"

#include "se/engine.hpp"
#include "se/graphics/geometry.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/renderManager.hpp"
#include "se/graphics/shader.hpp"
#include "se/graphics/shaderProgram.hpp"
#include "se/graphics/imageTexture.hpp"

#include "se/util/log.hpp"

#include <string.h>

using namespace se::entity;
using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

// ====================
// == PUBLIC MEMBERS ==
// ====================

Skybox::Skybox(se::Engine* engine, const char* texture) {
    this->engine = engine;
    this->texture_name = strdup(texture);
    this->texture = ImageTexture::get_texture(engine, texture);
    this->shader_program = 
        ShaderProgram::get_program(engine, "skybox", "", "skybox", "");
    this->geometry = Geometry::get_geometry(engine, "skybox");
    this->texture->increment_resource_user_counter();
    this->shader_program->increment_resource_user_counter();
    this->geometry->increment_resource_user_counter();
}

Skybox::~Skybox() {
    this->texture->decrement_resource_user_counter();
    this->shader_program->decrement_resource_user_counter();
    this->geometry->decrement_resource_user_counter();
    free((void*)this->texture_name);
}

void Skybox::render(glm::mat4 camera_matrix) {

    if(this->geometry->get_resource_state() != se::util::LoadableResourceState::LOADED ||
        this->texture->get_resource_state() != se::util::LoadableResourceState::LOADED ||
        this->shader_program->get_resource_state() != se::util::LoadableResourceState::LOADED){
        // Not ready to render
        DEBUG("Skybox [t: %s] not ready [t: %s s: %s g: %s]",
            this->texture_name,
            se::util::loadable_resource_state_name(this->texture->get_resource_state()),
            se::util::loadable_resource_state_name(this->geometry->get_resource_state()),
            se::util::loadable_resource_state_name(this->shader_program->get_resource_state()));
        return;
    }

    this->shader_program->use_program();
    this->texture->use_texture(GL_TEXTURE0);

    //glUniform1i(SE_SHADER_LOC_OUT_BG, SE_SHADER_LOC_OUT_BG);

    glUniformMatrix4fv(SE_SHADER_LOC_IN_MVP, 1, GL_FALSE, &camera_matrix[0][0]);
    /* The skybox does not have a model matrix because it is statically
    positioned in 3D space. */

    /* The depth buffer is used to determine when to show the skybox.  Writing
    to the buffer is disabled so that clipped values can be used to show the
    skybox. */
    glDepthMask(GL_FALSE);
    this->geometry->use_geometry();
    glDepthMask(GL_TRUE);
}