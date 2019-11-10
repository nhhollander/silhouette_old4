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

            /// OpenGL id for the target texture.
            unsigned int gl_texture_id = 0;

            /// OpenGL id for the background target texture.
            unsigned int gl_bg_texture_id = 0;

            /// OpenGL id for the depth buffer.
            unsigned int gl_depthbuffer_id = 0;

            /// OpenGL id for the depth texture.
            unsigned int gl_depth_texture_id = 0;

            /// Base texture unit is enabled.
            bool base_texture_enabled;

            /// Background texture unit is enabled.
            bool bg_texture_enabled;

            /// Depth texture unit is enabled.
            bool depth_texture_enabled;

            /// Texture wrapping mode
            GLenum texture_wrapping_mode;

            /// Pointer to the screen width.
            const volatile int* dimx;

            /// Pointer to the screen height.
            const volatile int* dimy;

            /// Pointer to the MSAA level.
            const volatile int* msaa_level;

            /// Pointer to the camera far distance
            const volatile float* cam_far;

            /// Pointer to the camera near distance
            const volatile float* cam_near;

            /// Status.
            FramebufferState state = FramebufferState::NOT_INITIALIZED;

            /// Framebuffer type
            FramebufferType type = FramebufferType::MULTISAMPLE;

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
             *  @param type     Framebuffer type
             *  @param base_texture_enabled
             *  @param bg_texture_enabled
             *  @param depth_texture_enalbed
             *  @param texture_wrapping_mode
             */
            Framebuffer(Engine* engine,
                FramebufferType type = FramebufferType::MULTISAMPLE,
                bool base_texture_enabled = true,
                bool bg_texture_enabled = true,
                bool depth_texture_enabled = true,
                GLenum texture_wrapping_mode = GL_REPEAT);

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
             */
            void use_as_texture();

    };

}

#endif