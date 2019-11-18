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

#include "GL/glew.h"

#include <vector>

namespace se::graphics {

    /// Framebuffer State
    enum class FramebufferState {
        NOT_INITIALIZED,
        INITIALIZING,
        INITIALIZED,
        DE_INITIALIZING,
        ERROR
    };

    /// Framebuffer type
    enum class FramebufferType {
        MULTISAMPLE,
        SINGLESAMPLE
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

            /// OpenGL id for the framebuffer.
            unsigned int gl_framebuffer_id = 0;

            /// Textures in the buffer
            std::vector<Texture*> textures;

            /// Status.
            FramebufferState state = FramebufferState::NOT_INITIALIZED;

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

            /// Re-initialize
            void re_init();

            /*!
             *  Create a new framebuffer.
             * 
             *  Buffers can be either multisampled or single-sampled.
             * 
             *  The three `*_texture_enabled` attributes determine which of the
             *  available render layers are allocated and used by this buffer.
             *  By default all available layers are enabled.
             * 
             *  The texture wrapping mode specifies how edge cases of the
             *  internal textures will be handled.  By default this value is set
             *  to `GL_REPEAT`.
             * 
             *  @param engine   Parent engine
             *  @param textures Textures to include in the buffer
             */
            Framebuffer(Engine* engine, std::vector<Texture*> textures);

            /// Destroy the framebuffer
            ~Framebuffer();

            /*!
             *  Use this framebuffer as a render target.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void use_as_target();

    };

}

#endif