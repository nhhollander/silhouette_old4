/*!
 *  @file src/se/entity.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity.hpp"

#include "util/log.hpp"

void se::Entity::render() {
    WARN("Renderable entity failed to override `render()`!");
}

void se::Entity::tick() {
    WARN("Tickable entity failed to override `tick()`!");
}