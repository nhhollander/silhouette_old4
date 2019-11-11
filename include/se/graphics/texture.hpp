/*!
 *  @file include/se/graphics/texture.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_TEXTURE_H_
#define _SE_GRAPHICS_TEXTURE_H_

#include "se/graphics/graphicsResource.hpp"

#include "se/fwd.hpp"

#include <GL/glew.h>

namespace se::graphics {

    /// Texture Sampling Type
    enum class TextureType {
        SINGLESAMPLE,
        MULTISAMPLE
    };

    /// Texture Construction Options
    struct TextureOptions {
        TextureType type = TextureType::SINGLESAMPLE;
        int mscount = 0;
        int dimx = 0;
        int dimy = 0;
        GLenum gl_mag_filter = GL_LINEAR;
        GLenum gl_min_filter = GL_LINEAR;
        GLenum gl_tex_wrap_s = GL_REPEAT;
        GLenum gl_tex_wrap_t = GL_REPEAT;
    };

    /*!
     *  Texture Class.
     */
    class Texture : public GraphicsResource {

        private:

            /*!
             *  Construct a new Texture.
             * 
             *  This function creates new entries in the graphics resource
             *  cache, and should be called paringly.  Textures should be
             *  obtained by calling the `get_texture()`.
             * 
             *  The `type` parameter is required to appropriately initialize the 
             */
            Texture(se::Engine* engine, const char* name);

        protected:

            /*!
             *  Construct a new Texture (for child classes).
             * 
             *  This function creates new entries in the graphics resource
             *  cache, and should be called paringly.  Textures should be
             *  obtained by calling the `get_texture()`.
             * 
             *  The hash parameter specifies which hash should be used when
             *  inserting this texture into the graphics resource cache.
             */
            Texture(se::Engine* engine, const char* name, uint32_t hash);

            /// Texture Name
            const char* name;

            /// Parent Engine
            se::Engine* engine;

            /*!
             *  Raw Texture Data.
             * 
             *  This data will be copied to the graphics device during the
             *  binding process.  It should already be in the format specified
             *  in the options structure.
             * 
             *  Setting this value to `nullptr` or `0` will initialize the
             *  texture as empty (black).
             */
            char* texture_data = nullptr;

            /// Texture Destructor
            ~Texture();

            /*!
             *  Bind texture data to the GPU.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            virtual void bind();

            /*!
             *  Unbind texture data from the GPU.
             * 
             *  **Warning:** This method must only be called from the garphics
             *  thread.
             */
            virtual void unbind();

            /// OpenGL texture ID
            unsigned int gl_texture = 0;

            /// @see graphicsResource.hpp
            virtual void load_();

            /// @see graphicsResource.hpp
            virtual void unload_();

        public:

            /*!
             *  Texture Options.
             * 
             *  Call `reload()` after changing these.
             */
            TextureOptions options;

            /*!
             *  Get a texture instance.
             * 
             *  This method attempts to retrieve a texture from the texture
             *  cache, and failing that instantiates a new object.
             * 
             *  Setting the `virt` flag to `true` indicates that this is a
             *  virtual texture which should not be loaded from disk.
             */
            static Texture* get_texture(se::Engine* engine, const char* name,
                bool virt = false);

            /*!
             *  Make this the active texture.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             *  
             *  @param tex_unit Texture unit to bind the texture to.
             */
            void use_texture(unsigned int tex_unit);

    };

}

#endif