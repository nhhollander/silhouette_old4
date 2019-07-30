/*!
 *  @file include/se/graphics/graphicsController.hpp
 * 
 *  Definitions for the OpenGL graphics subsystem.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GRAPHICSCONTROLLER_H_
#define _SE_GRAPHICS_GRAPHICSCONTROLLER_H_

#include "se/fwd.hpp"
#include "util/fwd.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <thread>
#include <functional>
#include <queue>

namespace se::graphics {

    /*!
     *  Render Thread Task.
     * 
     *  Represents a task that needs to be executed on the render thread.
     */
    typedef std::function<void(void)> GraphicsTask;

    /*!
    *  Graphics Controller.
    *
    *  The graphics controller is responsible for initializing OpenGL and
    *  managing the render pipeline.
    */
    class GraphicsController {

        private:

            /*!
             *  Parent Engine Instance.
             */
            se::Engine* engine = nullptr;

            /*
             *  SDL2 window object.
             */
            SDL_Window* window = nullptr;

            /*!
             *  OpenGL context.
             */
            SDL_GLContext glcontext;

            /*!
             *  Graphics Thread Object.
             */
            std::thread graphics_thread;

            /*!
             *  Graphics Support Thread Object.
             */
            std::thread graphics_support_thread;

            /*!
             *  Target frame time used for FPS limit.
             * 
             *  Negative values disable FPS cap.
             */
            int target_frame_time = -1;

            /*!
             *  Graphics Tasks.
             * 
             *  These tasks are executed in the order they are received, at a
             *  rate of one task per rendered frame.  While this does have a
             *  small impact on how quickly all tasks can be processed, it
             *  ensures that some frames still get rendered, preventing a
             *  complete output freeze if a lot of tasks land all at once.
             */
            std::queue<GraphicsTask> tasks;

            /*!
             *  Renderable Entities.
             */
            std::vector<se::Entity*> renderables;

            /// Active view point
            se::entity::Camera* active_camera;

            /*!
             *  Default camera object.
             * 
             *  When instantiated, the engine uses this as the value of
             *  `active_camera`.  It's kept around in case a scene is unloading
             *  and needs to get rid of a camera before the engine knows what
             *  the next camera is going to be, or during engine cleanup if the
             *  active camera is being destroyed.
             */
            se::entity::Camera* default_camera;

            /*!
             *  Graphics Thread.
             * 
             *  This method is spawned as the body of the graphics thread.  It
             *  will continue to run for as long as `se::engine::threads_run` is
             *  set to true.
             */
            void graphics_thread_main();

            /*!
             *  Render Loop.
             * 
             *  *Note: This method should only be called from the render loop*
             * 
             *  This method is called for each frame to be rendered, limited by
             *  `render.fpscap`
             */
            void render();

            /*!
             *  Graphics Support Thread.
             * 
             *  This thread is spawned as the body of the graphics support
             *  thread.  It does a variety of background tasks to optimize
             *  engine performance.
             */
            void graphics_support_thread_main();

            /*!
             *  Recalculate FPS limit.
             * 
             *  Invoke this method by updating the `render.fpscap` configuration
             *  value.
             */
            void recalculate_fps_limit(util::ConfigurationValue* value, util::Configuration* config);

            /*!
             *  Process pending graphics tasks.
             */
            void process_tasks();

            /*!
             *  Sort renderable entities.
             * 
             *  TODO: I'm not entirely sure how to balance the performance of
             *  this opeartion.  It does make rendering faster, but at the same
             *  time it takes a reasonable amount of processing power to
             *  calculate.
             */
            void sort_renderables();

        public:

            /*
             *  Construct a new graphics controller.
             */
            GraphicsController(se::Engine* engine);

            /// Destroy this graphics controller
            ~GraphicsController();

            /*!
             *  Submit a graphics task.
             * 
             *  Sometimes things need to be done on the graphics thread due to
             *  thread safety requirements, such as shader compilation or
             *  texture loading.
             */
            void submit_graphics_task(GraphicsTask task);

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