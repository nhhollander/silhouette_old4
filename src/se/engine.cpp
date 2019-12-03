/*!
 *  @file src/se/engine.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/engine.hpp"

#include "se/graphics/graphicsController.hpp"
#include "se/input/inputController.hpp"
#include "se/logic/logicController.hpp"

#include "se/util/log.hpp"
#include "se/util/config.hpp"
#include "se/util/dirs.hpp"

se::Engine::Engine() {
    // Load the base configuration
    DEBUG("Loading base configuration");
    this->config = new util::Configuration("base_config");
    std::string cfgfile = util::dirs::app_data();
    cfgfile += "/config.cfg";
    this->config->load(cfgfile.c_str());

    // Initialize inputs
    this->input_controller = new se::input::InputController(this);
    // Initialize graphics
    this->graphics_controller = new se::graphics::GraphicsController(this);
    // Initialize logic
    this->logic_controller = new se::logic::LogicController(this);

    INFO("Engine construction complete");
}

se::Engine::~Engine() {

    this->threads_run = false;

    delete this->graphics_controller;
    delete this->input_controller;
    delete this->logic_controller;
    
    INFO("Engine destruction complete");
}