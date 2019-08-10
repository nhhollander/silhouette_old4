/*!
 *  @file include/se/graphics/frameBuffer.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_FRAMEBUFFER_H_
#define _SE_GRAPHICS_FRAMEBUFFER_H_

#include "se/fwd.hpp"

namespace se::graphics {

    /// Framebuffer State
    enum class FramebufferState {
        NOT_INITIALIZED,
        INITIALIZING,
        INITIALIZED,
        DE_INITIALIZING,
        ERROR
    };

    /*!
     *  Framebuffer Class.
     * 
     *  Instead of rendering directly to the screen, rendering to a framebuffer
     *  allows post-processing to be applied to a render operation, or allows
     *  the result of a render operation to be stored for later use.
     * 
     *  Currently, framebuffers automatically assume the dimensions of the
     *  engine and are automatically resized to match.
     */
    class Framebuffer {

        private:

            /// Parent Engine.
            se::Engine* engine;

            /// Buffer Location
            unsigned int location;

            /// OpenGL id for the framebuffer.
            unsigned int gl_framebuffer_id = 0;

            /// OpenGL id for the target texture.
            unsigned int gl_texture_id = 0;

            /// OpenGL id for the depth buffer.
            unsigned int gl_depthbuffer_id = 0;

            /// Buffer Width.
            int dimx = 0;

            /// Buffer Height.
            int dimy = 0;

            /// MSAA level
            int msaa_level = 0;

            /// Status.
            FramebufferState state = FramebufferState::NOT_INITIALIZED;

            /// Re-initialize
            void re_init();

            /*!
             *  Framebuffer initialization method.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void init();

            /*!
             *  Framebuffer de-initialization method.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void deinit();
        
        public:

            /*!
             *  Create a new framebuffer.
             * 
             *  The `layout` parameter specifies which color attachment point
             *  the internal buffer will be bound to.  When rendering to this
             *  framebuffer, use the same location attribute in the shader
             *  program as was used when creating this buffer.
             * 
             *  For example, if you create this buffer with `location=4`, then
             *  your fragment shader should have
             *  `layout(location=5) out vec3 color`.  The resultant value of 
             *  color will be stored as a pixel in the image.
             * 
             *  @param engine   Parent engine
             *  @param location Output attachment location
             */
            Framebuffer(Engine* engine, unsigned int location);

            /// Destroy the framebuffer
            ~Framebuffer();

            /*!
             *  Use this framebuffer as a render target.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void use_as_target();

            /*!
             *  Use this framebuffer as a texture.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             * 
             *  @param unit Texture unit to bind the buffer texture to
             */
            void use_as_texture(unsigned int unit);

    };

}

#endif