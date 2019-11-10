/*!
 *  @file include/se/graphics/screen.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_SCREEN_H_
#define _SE_GRAPHICS_SCREEN_H_

#include "se/fwd.hpp"

namespace se::graphics {

    /*!
     *  Screen Class.
     * 
     *  The screen represents the post-processing stage in the rendering
     *  process.  After everything gets rendered to internal buffers, this
     *  class uses some kind of black magic to apply some pretty filters to the
     *  output, and render it to the screen.
     */
    class Screen {

        private:

            /// Parent Engine
            se::Engine* engine;

            /// Screen shader program
            se::graphics::ShaderProgram* screen_program;
            /// Screen post-process shading program
            se::graphics::ShaderProgram* post_process_program;

            /// OpenGL id for the screen vertex array
            unsigned int gl_screen_vert_array_id = 0;
            /// OpenGL id for the screen vertex buffer
            unsigned int gl_screen_vert_buffer_id = 0;

            /// Render framebuffer
            Framebuffer* primarybuffer;
            /// Post processing buffer
            Framebuffer* postprocessbuffer;

            /// Ready to rumble flag
            bool ready = false;

            /// Pointer to the output framebuffer ID
            const volatile int* output_fbid = nullptr;

            /*!
             *  Initialize the screen.
             * 
             *  **Warning:** This method must only be called from the grpahics
             *  thread.
             */
            void init();

            /*!
             *  De-Initialize the screen.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void deinit();

        public:

            /// Create a new screen
            Screen(se::Engine* engine);

            /// Destroy this screen
            ~Screen();

            /*!
             *  Activate the framebuffer.
             * 
             *  Activates the internal framebuffer so that rendering is directed
             *  there.
             */
            void activate_framebuffer();

            /*!
             *  Render the framebuffer to the screen.
             * 
             *  This method applies all post-processing effects and pushes the
             *  result to the screen.
             */
            void render();

    };

}

#endif