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
#include "se/entity/staticProp.hpp"
#include "se/entity/fpCamera.hpp"
#include "se/entity/skybox.hpp"
#include "se/entity/sign.hpp"
#include "se/graphics/graphicsController.hpp"
#include "se/graphics/simpleRenderManager.hpp"
#include "se/graphics/textTexture.hpp"

#include "se/util/config.hpp"
#include "se/util/log.hpp"

#include <thread>
#include <cstdlib>
#include <vector>

#ifdef SE_ENABLE_QT_WIDGET_SUPPORT
#include "mainwindow.h"
#include <QApplication>
#endif

using namespace se;
using namespace se::entity;
using namespace se::graphics;

int main(int argc, char** argv) {
    se::util::log::set_thread_name("MAIN");
    INFO("Hello World!");

    Engine e;

    SimpleRenderManager srm(&e);
    e.graphics_controller->set_render_manager(&srm);

    Scene scene(&e);
    scene.load_scene("test");
    srm.set_active_scene(&scene);

    FPCamera cam(&e);
    cam.set_name("camera");
    cam.z = 1.524;
    srm.set_active_camera(&cam);
    scene.register_entity(&cam);

    Skybox sb(&e, "skybox");
    sb.set_name("skybox");
    scene.register_entity(&sb);

    TextTexture tt(&e, "");
    tt.set_texturesize(300, 300);
    tt.update_text("Hello World!\nBeep Boop!\ntestΔ");

    Sign sign(&e,"sign");
    sign.set_text("Hello World");
    scene.register_entity(&sign);

    int counter = 0;
    if(e.config->get_bool("render.use_sdl")) {
        while(e.threads_run) {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            std::string msg = ">";
            msg += std::to_string(counter);
            counter++;
            sign.set_text(msg);
        }
    } else {

        #ifdef SE_ENABLE_QT_WIDGET_SUPPORT
        QApplication qapp(argc, argv);

        MainWindow w(&e, &scene);
        w.show();

        DEBUG("Starting application");
        return qapp.exec();
        #else
        FATAL("Attempted to create QT window, but QT libraries were not found when buildilng!");
        #endif
    }

}