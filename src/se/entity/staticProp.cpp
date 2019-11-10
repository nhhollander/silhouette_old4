/*!
 *  @file src/se/entity/staticProp.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity/staticProp.hpp"

#include "se/engine.hpp"
#include "se/graphics/geometry.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/renderManager.hpp"
#include "se/graphics/shader.hpp"
#include "se/graphics/shaderProgram.hpp"
#include "se/graphics/imageTexture.hpp"

#include "util/log.hpp"

#include <string.h>

using namespace se::entity;
using namespace se::graphics;

StaticProp::StaticProp(se::Engine* engine, const char* model, const char* texture) {
    this->engine = engine;
    this->model_name = strdup(model);
    this->texture_name = strdup(texture);
    this->geometry = se::graphics::Geometry::get_geometry(engine, model);
    this->texture = se::graphics::ImageTexture::get_texture(engine, texture);
    this->shader_program = se::graphics::ShaderProgram::get_program(
        engine, "static_prop", "", "static_prop", "");
    this->geometry->increment_active_users();
    this->texture->increment_active_users();
    this->shader_program->increment_active_users();
}

StaticProp::~StaticProp() {
    this->geometry->decrement_active_users();
    this->texture->decrement_active_users();
    this->shader_program->decrement_active_users();
    free((void*)this->model_name);
    free((void*)this->texture_name);
}

void StaticProp::render(glm::mat4 camera_matrix) {

    if(this->geometry->get_resource_state() != GraphicsResourceState::LOADED ||
        this->texture->get_resource_state() != GraphicsResourceState::LOADED ||
        this->shader_program->get_resource_state() != GraphicsResourceState::LOADED){
        // Not ready to render
        DEBUG("Static prop [m: %s t: %s] not ready [m: %s t: %s s: %s]",
            this->model_name, this->texture_name,
            graphics_resource_state_name(this->geometry->get_resource_state()),
            graphics_resource_state_name(this->texture->get_resource_state()),
            graphics_resource_state_name(this->shader_program->get_resource_state()));
        return;
    }

    //DEBUG("Rendering static prop [m: %s t: %s]",
    //    this->model_name, this->texture_name);

    this->shader_program->use_program();
    this->texture->use_texture(GL_TEXTURE0);

    glm::mat4 model_matrix = this->get_model_matrix();
    glm::mat4 mvp_matrix = camera_matrix * model_matrix;
    glUniformMatrix4fv(SE_SHADER_LOC_IN_MVP, 1, GL_FALSE, &mvp_matrix[0][0]);
    glUniformMatrix4fv(SE_SHADER_LOC_IN_MODEL_MAT, 1, GL_FALSE, &model_matrix[0][0]);

    this->geometry->use_geometry();

}