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
#include "se/graphics/shaderProgram.hpp"
#include "se/graphics/texture.hpp"

#include "util/log.hpp"

#include <string.h>

using namespace se::entity;

StaticProp::StaticProp(se::Engine* engine, const char* model, const char* texture) {
    this->geometry = se::graphics::Geometry::get_geometry(engine, model);
    this->texture = se::graphics::Texture::get_texture(engine, texture);
    this->geometry->increment_active_users();
    this->texture->increment_active_users();
    this->shader_program = se::graphics::ShaderProgram::get_program(engine, "static_prop", "static_prop");
    this->engine = engine;
    this->model_name = strdup(model);
    this->texture_name = strdup(model);
    this->engine->graphics_controller->add_renderable(this);
}

StaticProp::~StaticProp() {
    this->engine->graphics_controller->remove_renderable(this);
    this->geometry->decrement_active_users();
    this->texture->decrement_active_users();
    delete[] this->model_name;
    delete[] this->texture_name;
}

void StaticProp::render() {
    DEBUG("Rendering static prop [m: %s t: %s]",
        this->model_name, this->texture_name);
}