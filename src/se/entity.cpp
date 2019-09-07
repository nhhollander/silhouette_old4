/*!
 *  @file src/se/entity.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity.hpp"

#include "util/log.hpp"

#include <cstdio>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// =====================
// == VIRTUAL MEMBERS ==
// =====================

void se::Entity::render(glm::mat4 camera_matrix) {
    WARN("Renderable entity failed to override `render()`!");
}

void se::Entity::tick() {
    WARN("Tickable entity failed to override `tick()`!");
}

// =========================
// == NON-VIRTUAL MEMBERS ==
// =========================

se::Entity::Entity() {
    this->name = strdup("<name_not_set>");
}

se::Entity::~Entity() {
    free((void*) this->name);
}

#define MATRIX(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) { \
    {a,e,i,m}, \
    {b,f,j,n}, \
    {c,g,k,o}, \
    {d,h,l,p} \
}

glm::mat4 se::Entity::get_model_matrix() {

    glm::mat4 translate = MATRIX(
        1.0, 0.0, 0.0, this->x,
        0.0, 1.0, 0.0, this->y,
        0.0, 0.0, 1.0, this->z,
        0.0, 0.0, 0.0, 1.0
    );

    float xcos = cos(this->rx);
    float xsin = sin(this->rx);
    glm::mat4 rotate_x = MATRIX(
        1.0,  0.0,   0.0, 0.0,
        0.0, xcos, -xsin, 0.0,
        0.0, xsin,  xcos, 0.0,
        0.0,  0.0,   0.0, 1.0
    );

    float ycos = cos(this->ry);
    float ysin = sin(this->ry);
    glm::mat4 rotate_y = MATRIX(
         ycos, 0.0, ysin, 0.0,
          0.0, 1.0,  0.0, 0.0,
        -ysin, 0.0, ycos, 0.0,
          0.0, 0.0,  0.0, 1.0
    );

    float zcos = cos(this->rz);
    float zsin = sin(this->rz);
    glm::mat4 rotate_z = MATRIX(
        zcos, -zsin, 0.0, 0.0,
        zsin,  zcos, 0.0, 0.0,
         0.0,   0.0, 1.0, 0.0,
         0.0,   0.0, 0.0, 1.0
    );

    glm::mat4 scale = MATRIX(
        this->sx, 0.0, 0.0, 0.0,
        0.0, this->sy, 0.0, 0.0,
        0.0, 0.0, this->sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    return translate * rotate_z * rotate_y * rotate_x * scale;

}

void se::Entity::set_name(const char* name) {
    free((void*) this->name);
    this->name = strdup(name);
}

const char* se::Entity::get_name() {
    return this->name;
}