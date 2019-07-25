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
#include "se/graphics/shaderProgram.hpp"

#include <thread>

using namespace se;

int main() {
    util::log::set_thread_name("MAIN");
    INFO("Hello World!");

    Engine e;
    graphics::ShaderProgram* prog = graphics::ShaderProgram::get_program(&e, "test", "test");
    prog->wait_for_loading();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    INFO("Test shader program complete!");
}