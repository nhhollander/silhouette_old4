/*!
 *  @file src/se/engine.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/engine.hpp"

#include "util/log.hpp"
#include "util/config.hpp"

se::Engine::Engine() {
    // Load the base configuration
    DEBUG("Loading base configuration");
    this->config = new util::Configuration("base_config");
    this->config->load("config.json");

    INFO("Engine construction complete");
}

se::Engine::~Engine() {
    INFO("Engine destruction complete");
}