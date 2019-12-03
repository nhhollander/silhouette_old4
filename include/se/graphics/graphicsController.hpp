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
#include "se/graphics/graphicsEventHandler.hpp"

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

        /*!
         *  Allow the graphics event management class to take care of all of our
         *  internal stuff from a separate class.  That should help keep this
         *  class a bit neater.
         */
        friend class GraphicsEventHandler;

        #ifdef SE_ENABLE_QT_WIDGET_SUPPORT
        /*!
         *  Allow the QT Silhouette Widget to administer the render loop.
         */
        friend class QTSilhouetteWidget;
        #endif

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
             *  Target frame time used for FPS limit.
             * 
             *  Negative values disable FPS cap.
             */
            int64_t target_frame_time = -1;

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
             *  Graphics Event Handler.
             * 
             *  This takes care of graphics events so that this class doesn't
             *  have to.  TOOD: Maybe this is a stupid idea and it should be
             *  merged into this class?  Time shall tell.
             */
            GraphicsEventHandler* event_handler;

            /*!
             *  Render Manager.
             * 
             *  A render manager must be used to deal with that crazy rendering
             *  business that's too tubular to put in this class.
             */
            RenderManager* render_manager = nullptr;

            /*!
             *  Graphics Thread.
             * 
             *  This method is spawned as the body of the graphics thread.  It
             *  will continue to run for as long as `se::engine::threads_run` is
             *  set to true.
             */
            void graphics_thread_main();

            /*!
             *  Do Frame.
             * 
             *  This method exists to decouple the frame rendering process from
             *  the default control loop.  It was implemented to allow the use
             *  of an embedded OpenGL widget in a QT application, which
             *  overrides the render loop handling.
             */
            void do_frame();

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
             *  SDL Initialization Method.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             * 
             *  This method handles the initialization process for SDL,
             *  including window creation and OpenGL context creation.
             */
            void init_sdl();

            /*!
             *  OpenGL Initialization Method.
             * 
             *  **Warning:** This method must only be called from the grpahics
             *  thread.
             * 
             *  This method handles the initialization/setup process for OpenGL
             *  once a context has been provided (by SDL or otherwise).
             */
            void init_gl();

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

            /// Specify the active render manager
            void set_render_manager(se::graphics::RenderManager* manager);

            /// Get the active render manager
            se::graphics::RenderManager* get_render_manager();

            /*!
             *  Pending Graphics Tasks.
             * 
             *  Returns the number of pending graphics tasks.
             */
            int pending_task_count();
    };

}

#endif