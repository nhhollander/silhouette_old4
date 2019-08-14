/*!
 *  @file src/se/graphics/geometry.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/graphics/geometry.hpp"

#include "se/engine.hpp"
#include "se/graphics/shader.hpp"
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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace se::graphics;

// =====================
// == PRIVATE MEMBERS ==
// =====================

#define GEOM_HASH_FORMAT "geometry:%p:%s"

Geometry::Geometry(se::Engine* engine, const char* name) :
    GraphicsResource(util::hash::ejenkins(GEOM_HASH_FORMAT, engine, name)) {
    this->engine = engine;
    this->name = strdup(name);
}

Geometry::~Geometry() {
    FATAL("Geometry destroyed");
    free((void*)this->name);
}

void Geometry::bind() {

    glGenVertexArrays(1, &this->gl_vertex_array_object_id);
    glBindVertexArray(this->gl_vertex_array_object_id);

    glGenBuffers(1, &this->gl_vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->gl_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, this->vertex_data.size() * sizeof(glm::vec3),
        &this->vertex_data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(SE_SHADER_LOC_IN_VERT, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SE_SHADER_LOC_IN_VERT);

    glGenBuffers(1, &this->gl_uv_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->gl_uv_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, this->uv_data.size() * sizeof(glm::vec2),
        &this->uv_data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(SE_SHADER_LOC_IN_UV, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SE_SHADER_LOC_IN_UV);

    glGenBuffers(1, &this->gl_normal_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, this->gl_normal_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, this->normal_data.size() * sizeof(glm::vec3),
        &this->normal_data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(SE_SHADER_LOC_IN_NORM, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(SE_SHADER_LOC_IN_NORM);

    this->vertex_data.clear();
    this->uv_data.clear();
    this->normal_data.clear();

    DEBUG("Geometry [%s] bound successfuly as [%u]!", this->name,
        this->gl_vertex_array_object_id);
    this->resource_state = GraphicsResourceState::LOADED;

}

void Geometry::unbind() {
    
    glDeleteBuffers(1, &this->gl_vertex_buffer_id);
    glDeleteBuffers(1, &this->gl_uv_buffer_id);
    glDeleteBuffers(1, &this->gl_normal_buffer_id);

    this->gl_vertex_buffer_id = 0;
    this->gl_uv_buffer_id = 0;
    this->gl_normal_buffer_id = 0;

    DEBUG("Geometry [%s] unbound", this->name);
}

// =======================
// == PROTECTED METHODS ==
// =======================

void Geometry::load_() {
    DEBUG("Loading geometry [%s]", this->name);
    this->resource_state = GraphicsResourceState::LOADING;

    /* This OBJ loader was adapted from the awesome folks at
    http://www.opengl-tutorial.org/ */
    std::vector<unsigned int> vertex_indices;
    std::vector<unsigned int> uv_indices;
    std::vector<unsigned int> normal_indices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::string fname = util::dirs::app_data();
    fname += "/models/";
    fname += name;
    fname += ".obj";

    FILE* fp = fopen(fname.c_str(), "r");
    if(fp == nullptr) {
        ERROR("[%s] Failed to open model file [%s] [%i: %s]",
            this->name, fname.c_str(), errno, strerror(errno));
        this->resource_state = GraphicsResourceState::ERROR;
        return;
    }

    // It is time to read the OBJ file

    while(true) {

        char line_header[128];
        int res = fscanf(fp, "%127s", line_header);
        if(res == EOF) {
            break;
        }

        if(strcmp(line_header, "v") == 0) {
            // This line defines a vertex
            glm::vec3 vertex;
            fscanf(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        } else if(strcmp(line_header, "vt") == 0) {
            // This line defines a uv coordinate
            glm::vec2 uv;
            fscanf(fp, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        } else if(strcmp(line_header, "vn") == 0) {
            // This line defines a normal
            glm::vec3 normal;
            fscanf(fp, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        } else if(strcmp(line_header, "f") == 0) {
            // This line defines a face
            unsigned int vertex_index[3];
            unsigned int uv_index[3];
            unsigned int normal_index[3];
            int matches = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertex_index[0], &uv_index[0], &normal_index[0],
                &vertex_index[1], &uv_index[1], &normal_index[1],
                &vertex_index[2], &uv_index[2], &normal_index[2]);
            if(matches != 9) {
                ERROR("[%s] OBJ format unsupported [%s]",
                    this->name, fname.c_str());
                this->resource_state = GraphicsResourceState::ERROR;
                return;
            }
            vertex_indices.push_back(vertex_index[0]);
            vertex_indices.push_back(vertex_index[1]);
            vertex_indices.push_back(vertex_index[2]);
            uv_indices    .push_back(uv_index[0]);
            uv_indices    .push_back(uv_index[1]);
            uv_indices    .push_back(uv_index[2]);
            normal_indices.push_back(normal_index[0]);
            normal_indices.push_back(normal_index[1]);
            normal_indices.push_back(normal_index[2]);
        }

    }

    this->vertex_data.clear();
    this->uv_data.clear();
    this->normal_data.clear();

    // It is time to parse the OBJ data
    for(unsigned int i = 0; i < vertex_indices.size(); i++) {
        unsigned int vertex_index = vertex_indices[i];
        glm::vec3 vertex = temp_vertices[vertex_index - 1];
        this->vertex_data.push_back(vertex);
    }
    for(unsigned int i = 0; i < uv_indices.size(); i++) {
        unsigned int uv_index = uv_indices[i];
        glm::vec2 uv = temp_uvs[uv_index - 1];
        this->uv_data.push_back(uv);
    }
    for(unsigned int i = 0; i < normal_indices.size(); i++) {
        unsigned int normal_index = normal_indices[i];
        glm::vec3 normal = temp_normals[normal_index - 1];
        this->normal_data.push_back(normal);
    }

    this->vertex_array_size = this->vertex_data.size();

    DEBUG("Loaded [%s], waiting for bind", this->name);
    std::function job = [this](){this->bind();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

void Geometry::unload_() {
    DEBUG("Unloading [%s], waiting for unbind", this->name);
    this->resource_state = GraphicsResourceState::NOT_LOADED;
    std::function job = [this](){this->unbind();};
    this->engine->graphics_controller->submit_graphics_task(job);
}

// ====================
// == PUBLIC METHODS ==
// ====================

Geometry* Geometry::get_geometry(se::Engine* engine, const char* name) {
    uint32_t hash = util::hash::ejenkins(GEOM_HASH_FORMAT, engine, name);
    GraphicsResource* resource = Geometry::get_resource(hash);
    if(resource == nullptr) {
        DEBUG("Geometry [%s] not in cache :(", name);
        return new Geometry(engine, name);
    }
    DEBUG("Found geometry [%s] in cache!", name);
    return static_cast<Geometry*>(resource);
}

void Geometry::use_geometry() {
    if(this->resource_state != GraphicsResourceState::LOADED) { return; }
    glBindVertexArray(this->gl_vertex_array_object_id);
    glDrawArrays(GL_TRIANGLES, 0, this->vertex_array_size);
}