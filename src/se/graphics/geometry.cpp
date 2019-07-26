/*!
 *  @file src/se/graphics/geometry.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/geometry.hpp"

#include "se/engine.hpp"
#include "se/graphics/graphicsController.hpp"

#include "util/dirs.hpp"
#include "util/hash.hpp"
#include "util/log.hpp"
#include "util/debugstrings.hpp"

#include <string.h>
#include <string>
#include <png.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

Geometry::Geometry(se::Engine* engine, const char* name) :
    GraphicsResource(util::hash::ejenkins("%p:%s", engine, name)) {
    this->engine = engine;
    this->name = strdup(name);
}

Geometry::~Geometry() {
    delete[] this->name;
}

void Geometry::bind() {
    DEBUG("TODO: Bind geometry [%s]", this->name);
}

void Geometry::unbind() {
    DEBUG("TODO: Unbind geometry [%s]", this->name);
}

// =======================
// == PROTECTED METHODS ==
// =======================

void Geometry::load_() {
    DEBUG("Loading geometry [%s]", this->name);
    this->resource_state = GraphicsResourceState::LOADING;

    DEBUG("TODO: Load geometry [%s]", this->name);
}

void Geometry::unload_() {
    DEBUG("Unloading geometry [%s]", this->name);
    this->resource_state = GraphicsResourceState::NOT_LOADED;

    DEBUG("TODO: Unload geometry [%s]", this->name);
}

// ====================
// == PUBLIC METHODS ==
// ====================

Geometry* Geometry::get_geometry(se::Engine* engine, const char* name) {
    uint32_t hash = util::hash::ejenkins("geometry:%p:%s", engine, name);
    GraphicsResource* resource = Geometry::get_resource(hash);
    if(resource == nullptr) {
        DEBUG("Geometry [%s] not in cache :(", name);
        return new Geometry(engine, name);
    }
    DEBUG("Found geometry [%s] in cache!", name);
    return static_cast<Geometry*>(resource);
}