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
#include "se/scene.hpp"
#include "util/log.hpp"
#include "se/entity/staticProp.hpp"
#include "se/entity/fpCamera.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/simpleRenderManager.hpp"

#include <thread>
#include <cstdlib>
#include <vector>

using namespace se;
using namespace se::entity;
using namespace se::graphics;

int main() {
    util::log::set_thread_name("MAIN");
    INFO("Hello World!");

    Engine e;

    SimpleRenderManager srm(&e);
    e.graphics_controller->set_render_manager(&srm);

    Scene scene(&e);
    scene.load_scene("test");
    srm.set_active_scene(&scene);

    /*StaticProp floor(&e, "floor", "floor");
    floor.x = 0;
    floor.y = 0;
    floor.z = 0;
    scene.register_entity(&floor);

    StaticProp sp(&e, "suzanne", "suzanne");
    sp.x = 0;
    sp.y = 3;
    sp.z = 1;
    scene.register_entity(&sp); */

    FPCamera cam(&e);
    cam.z = 1.524;
    cam.lock_mouse();
    srm.set_active_camera(&cam);

    while(e.threads_run) {
        //sp.rz += 0.005;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }


}