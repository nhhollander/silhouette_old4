/*!
 *  @file include/se/graphics/imageTexture.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_IMAGETEXTURE_H_
#define _SE_GRAPHICS_IMAGETEXTURE_H_

#include "se/graphics/texture.hpp"

#include "util/cacheableResource.hpp"

namespace se::graphics {

    /*!
     *  Image Texture Class.
     * 
     *  Image textures are populated with data loaded from PNG or other
     *  image files.
     */
    class ImageTexture : public Texture, public util::CacheableResource {

        private:

            /*!
             *  Construct a new ImageTexture.
             * 
             *  This function creates new entries in the graphics resource
             *  cache, and should be called paringly.  Textures should be
             *  obtained by calling the `get_texture()`.
             */
            ImageTexture(se::Engine* engine, const char* name);

            /// Texture Destructor
            ~ImageTexture();

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

        protected:

            void load_();

            void unload_();

            uint32_t resource_id();

            std::string resource_name();

        public:

            /*!
             *  Get a texture instance.
             * 
             *  This method attempts to retrieve a texture from the texture
             *  cache, and failing that instantiates a new object.
             */
            static ImageTexture* get_texture(se::Engine* engine, const char* name);

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