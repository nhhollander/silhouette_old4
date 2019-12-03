/*!
 *  @file src/se/entity/sign.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity/sign.hpp"

#include "se/engine.hpp"
#include "se/graphics/geometry.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/renderManager.hpp"
#include "se/graphics/shader.hpp"
#include "se/graphics/shaderProgram.hpp"
#include "se/graphics/imageTexture.hpp"
#include "se/graphics/textTexture.hpp"

#include "se/util/log.hpp"

#include <string.h>

using namespace se::entity;
using namespace se::graphics;

Sign::Sign(se::Engine* engine, const char* model) {
    this->engine = engine;
    this->model_name = strdup(model);
    this->geometry = se::graphics::Geometry::get_geometry(engine, model);
    this->texture = new se::graphics::TextTexture(engine, "sign_texture");
    this->shader_program = se::graphics::ShaderProgram::get_program(
        engine, "static_prop", "", "static_prop", "");
    this->geometry->increment_resource_user_counter();
    this->shader_program->increment_resource_user_counter();
}

Sign::~Sign() {
    this->geometry->decrement_resource_user_counter();
    this->shader_program->decrement_resource_user_counter();
    free((void*)this->model_name);
}

void Sign::render(glm::mat4 camera_matrix) {

    if(this->geometry->get_resource_state() != util::LoadableResourceState::LOADED ||
        this->texture->get_resource_state() != util::LoadableResourceState::LOADED ||
        this->shader_program->get_resource_state() != util::LoadableResourceState::LOADED){
        // Not ready to render
        return;
    }

    this->shader_program->use_program();
    this->texture->use_texture(GL_TEXTURE0);

    glm::mat4 model_matrix = this->get_model_matrix();
    glm::mat4 mvp_matrix = camera_matrix * model_matrix;
    glUniformMatrix4fv(SE_SHADER_LOC_IN_MVP, 1, GL_FALSE, &mvp_matrix[0][0]);
    glUniformMatrix4fv(SE_SHADER_LOC_IN_MODEL_MAT, 1, GL_FALSE, &model_matrix[0][0]);

    this->geometry->use_geometry();

}

void Sign::set_text(std::string text) {
    this->texture->update_text(text);
}