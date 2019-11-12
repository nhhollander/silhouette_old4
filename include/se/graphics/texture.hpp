/*!
 *  @file include/se/graphics/texture.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_TEXTURE_H_
#define _SE_GRAPHICS_TEXTURE_H_

#include "se/fwd.hpp"

#include "se/graphics/graphicsResource.hpp"

#include <GL/glew.h>

namespace se::graphics {

    /*
     *  Texture Options.
     * 
     * | Parameter         | Description                                       |
     * |-------------------|---------------------------------------------------|
     * |`type`             |Texture type, `GL_TEXTURE_2D` for singlesample, `GL_TEXTURE_2D_MULTISAMPLE` for multisample|
     * |`mscount`          |Multisample count, only applicable when `gl_type` is `GL_TEXTURE_2D_MULTISAMPLE`|
     * |`dimx`             |Image width                                        |
     * |`dimy`             |Image height                                       |
     * |`gl_mag_filter`    |Texture magnification function.  [See khronos documentation.][1]|
     * |`gl_min_filter`    |Texture minification function.  [See khronos documentation.][1]|
     * |`gl_tex_wrap_s`    |S-coordinate wrapping function.  [See khronos documentation.][1]|
     * |`gl_tex_wrap_t`    |T-coordinate wrapping function.  [See khronos documentation.][1]|
     * |`gl_color_attachment`|Color attachment point.  Leave as 0 for no attachment.  [See khronos documentation.][2]|
     * |`gl_color_format`  |Internal color format of the texture.  [See khronos documentation.][3]|
     * |`gl_data_format`   |Color format of the raw texture data.  [See khronos documentation.][3]|
     * 
     * [1]: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
     * [2]: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glFramebufferTexture.xhtml
     * [3]: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
     */
    struct TextureOptions {
        GLenum type = GL_TEXTURE_2D;
        int mscount = 0;
        int dimx = 0;
        int dimy = 0;
        GLenum gl_mag_filter = GL_LINEAR;
        GLenum gl_min_filter = GL_LINEAR;
        GLenum gl_tex_wrap_s = GL_REPEAT;
        GLenum gl_tex_wrap_t = GL_REPEAT;
        GLenum gl_color_attachment = 0;
        GLenum gl_color_format = GL_RGB;
        GLenum gl_data_format = GL_RGB;
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
             */
            static Texture* get_texture(se::Engine* engine, const char* name);

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