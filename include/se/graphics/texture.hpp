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

namespace se::graphics {

    /*!
     *  Texture Class.
     * 
     *   */
    class Texture : public GraphicsResource {

        private:

            /// Texture Name
            const char* name;

            /// Parent Engine
            se::Engine* engine;

            /*!
             *  Construct a new Texture.
             * 
             *  Where possible, it is preferred to retrieve textures from the
             *  resource cache instead of constructing duplicates.
             */
            Texture(se::Engine* engine, const char* name);

            /// Texture Destructor
            ~Texture();

            /*!
             *  Raw Decoded Texture Data.
             * 
             *  Contains the raw pixels as read from whatever image format this
             *  texture was in.
             */
            char* raw_texture = nullptr;

            /*!
             *  Bind texture data to the GPU.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void bind();

            /*!
             *  Unbind texture data from the GPU.
             * 
             *  **Warning:** This method must only be called from the garphics
             *  thread.
             */
            void unbind();

            /// Image Width
            int width = 0;
            /// Image Height
            int height = 0;

            /// OpenGL texture ID
            unsigned int gl_texture = 0;

        protected:

            void load_();

            void unload_();

        public:

            static Texture* get_texture(se::Engine* engine, const char* name);

    };

}

#endif