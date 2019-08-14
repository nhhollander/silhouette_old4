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

    StaticProp floor(&e, "floor", "floor");
    floor.x = 0;
    floor.y = 0;
    floor.z = 0;
    srm.add_renderable(&floor);

    StaticProp sp(&e, "suzanne", "suzanne");
    sp.x = 0;
    sp.y = 3;
    sp.z = 1;
    srm.add_renderable(&sp);

    FPCamera cam(&e);
    cam.z = 1.524;
    srm.set_active_camera(&cam);

    while(e.threads_run) {
        //sp->z -= 0.01;
        //cam->rz += 0.01;
        //sp->ry -= 0.1;
        //sp->y += 0.01;
        //cam->rz += 0.05;
        //cam->x += 0.01;
        sp.rz += 0.005;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }


}

// Test Commit Please Ignore