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

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <thread>

namespace se::graphics {

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
             *  `render.fps_cap`
             */
            void render();

        public:

            /*
             *  Construct a new graphics controller.
             */
            GraphicsController(se::Engine* engine);

            /// Destroy this graphics controller
            ~GraphicsController();
    };

}

#endif