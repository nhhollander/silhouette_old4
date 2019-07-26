/*!
 *  @file src/se/engine.cpp
 *
 *  Test Impelementation used for Development
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/engine.hpp"
#include "util/log.hpp"
#include "se/graphics/geometry.hpp"

#include <thread>

using namespace se;

int main() {
    util::log::set_thread_name("MAIN");
    INFO("Hello World!");

    Engine e;

    se::graphics::Geometry* geom = graphics::Geometry::get_geometry(&e, "suzanne");
    DEBUG("Incrementing active users to cause test model load");
    geom->increment_active_users();
    while(geom->get_resource_state() != se::graphics::GraphicsResourceState::LOADED) {}
    DEBUG("Decrementing active users to cause test geometry unload");
    geom->decrement_active_users();

    std::this_thread::sleep_for(std::chrono::seconds(2));
}