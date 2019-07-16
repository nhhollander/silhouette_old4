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

using namespace se;

int main() {
    util::log::set_thread_name("MAIN");
    INFO("Hello World!");

    Engine e;
}