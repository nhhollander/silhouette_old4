/*!
 *  @file include/se/graphics/simpleRenderManager.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_SIMPLERENDERMANAGER_H_
#define _SE_GRAPHICS_SIMPLERENDERMANAGER_H_

#include "se/fwd.hpp"
#include "se/graphics/renderManager.hpp"

#include <vector>
#include <thread>

namespace se::graphics {

    /*!
     *  Simple Render Manager.
     * 
     *  Default render manager used for testing and development.  Unless you're
     *  doing something fancy, this is probably the render manager that you want
     *  to be using.
     * 
     *  This render manager designates a 3D world with a variable camera, with
     *  post processing.
     */
    class SimpleRenderManager : public RenderManager {

        private:

            /// Parent Engine
            se::Engine* engine;

            /// Current Scene
            se::Scene* active_scene;

            /// Default scene
            se::Scene* default_scene;

            /// Active view point
            se::entity::Camera* active_camera;

            /// Default camera
            se::entity::Camera* default_camera;

            /// Screen used for output rendering
            se::graphics::Screen* screen;

            /// Support thread.
            std::thread support_thread;

            /// Threads run flag
            bool run = true;

            /*!
             *  Graphics Support Thread.
             * 
             *  This thread is spawned to perform a variety of background tasks
             *  related to bookkeeping and realtime optimization.
             */
            void support_thread_main();

            /*!
             *  Sort Renderable Entities.
             * 
             *  One of several support thread tasks.
             */
            void sort_renderables();

        public:

            /// Construct a new simple render manager.
            SimpleRenderManager(se::Engine* engine);

            /// Destroy this simple render manager
            ~SimpleRenderManager();

            /// Render a frame
            void render_frame();

            /*!
             *  Set the active camera.
             * 
             *  Don't delete the camera passed here until you call
             *  `use_default_camera()`.
             */
            void set_active_camera(se::entity::Camera* camera);

            /*!
             *  Revert to the default camera.
             * 
             *  Call this method before deleting cameras to prevent segmentation
             *  faults.
             */
            void use_default_camera();

            /*!
             *  Set the active scene.
             * 
             *  Don't delete the scene passed here until you call
             *  `use_default_scene()`.
             */
            void set_active_scene(se::Scene* scene);

            /*!
             *  Revert to the default scene.
             * 
             *  Call this method before deleting scenes to prevent segmentation
             *  faults.
             */
            void use_default_scene();

    };

}

#endif