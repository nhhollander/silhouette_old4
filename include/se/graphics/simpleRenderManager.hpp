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

            /// Renderable entities.
            std::vector<se::Entity*> renderables;

            /// Active view point
            se::entity::Camera* active_camera;

            /// Screen used for output rendering
            se::graphics::Screen* screen;

            /// Support thread.
            std::thread support_thread;

            /// Default camera
            se::entity::Camera* default_camera;

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
             *  Add a renderable entity.
             * 
             *  Entities in this list will be called upon by the render thread
             *  every frame.  The more entities in the render queue, the slower
             *  the engine is going to run.  If an entity is no longer a
             *  candidate for rendering, please remove it by calling
             *  `remove_renderable()`.
             * 
             *  Only add entities for which `se::Entity::is_renderable()`
             *  returns `true`.
             * 
             *  TODO: Render groups.  Instead of worrying about dynamic 
             *  visibility, having groups of entities pre-categorized into
             *  render groups in some kind of tree would be a good way of
             *  keeping performance high.  I believe this is sort of how other
             *  engines (like Source) deal with props and entities, so that they
             *  don't have to worry about excessive calls to nearby but
             *  invisible entities.
             */
            void add_renderable(se::Entity* entity);

            /*!
             *  Remove a renderable entity.
             * 
             *  When an entity is no longer renderable, removing it from the
             *  list of renderable entities will help keep the engine running
             *  as quickly as possible.
             */
            void remove_renderable(se::Entity* entity);

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

    };

}

#endif