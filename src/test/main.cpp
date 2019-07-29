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
#include "se/entity/staticProp.hpp"

#include <thread>
#include <cstdlib>
#include <vector>

using namespace se;
using namespace se::entity;

int main() {
    util::log::set_thread_name("MAIN");
    INFO("Hello World!");

    Engine e;

    std::vector<StaticProp*> props;

    for(int i = 0; i < 1; i++) {
        StaticProp* prop = new StaticProp(&e, "suzanne", "test");
        prop->x = (rand() % 20) - 10.0;
        prop->y = (rand() % 20) - 10.0;
        prop->z = (rand() % 20) - 10.0;
        prop->x = 0;
        prop->y = 0;
        prop->z = 0;
        props.push_back(prop);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));


}