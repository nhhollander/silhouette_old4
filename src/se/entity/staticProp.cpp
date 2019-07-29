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
#include "se/graphics/shader.hpp"
#include "se/graphics/shaderProgram.hpp"
#include "se/graphics/texture.hpp"

#include "util/log.hpp"

#include <string.h>

using namespace se::entity;
using namespace se::graphics;

StaticProp::StaticProp(se::Engine* engine, const char* model, const char* texture) {
    this->geometry = se::graphics::Geometry::get_geometry(engine, model);
    this->texture = se::graphics::Texture::get_texture(engine, texture);
    this->geometry->increment_active_users();
    this->texture->increment_active_users();
    this->shader_program = se::graphics::ShaderProgram::get_program(
        engine, "static_prop", "", "static_prop", "");
    this->engine = engine;
    this->model_name = strdup(model);
    this->texture_name = strdup(texture);
    this->engine->graphics_controller->add_renderable(this);
}

StaticProp::~StaticProp() {
    this->engine->graphics_controller->remove_renderable(this);
    this->geometry->decrement_active_users();
    this->texture->decrement_active_users();
    delete[] this->model_name;
    delete[] this->texture_name;
}

void StaticProp::render(glm::mat4 camera_matrix) {

    if(this->geometry->get_resource_state() != GraphicsResourceState::LOADED ||
        this->texture->get_resource_state() != GraphicsResourceState::LOADED ||
        this->shader_program->get_state() != ShaderProgramState::READY){
        // Not ready to render
        DEBUG("Static prop [m: %s t: %s] not ready [m: %s t: %s s: %s]",
            this->model_name, this->texture_name,
            graphics_resource_state_name(this->geometry->get_resource_state()),
            graphics_resource_state_name(this->texture->get_resource_state()),
            shader_program_state_name(this->shader_program->get_state()));
        return;
    }

    //DEBUG("Rendering static prop [m: %s t: %s]",
    //    this->model_name, this->texture_name);

    this->shader_program->use_program();
    this->geometry->use_geometry();

    glm::mat4 mvp_matrix = camera_matrix * this->get_model_matrix();
    glUniformMatrix4fv(SE_SHADER_LOC_IN_MVP, 1, GL_FALSE, &mvp_matrix[0][0]);

}