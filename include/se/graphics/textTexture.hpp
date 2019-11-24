/*!
 *  @file include/se/graphics/textTexture.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_TEXTTEXTURE_H_
#define _SE_GRAPHICS_TEXTTEXTURE_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include "se/graphics/texture.hpp"

namespace se::graphics {

    /*!
     *  Text Texture Class.
     * 
     *  Text textures can have text rendered to them.  Pretty neat.
     */
    class TextTexture : public Texture {

        private:

            /// FreeType font Library handle
            FT_Library ft = nullptr;
            /// Font face
            FT_Face font_face = nullptr;

            /// Render origin X
            int origin_x = 3;
            /// Render origin Y
            int origin_y = 50;

            /*!
             *  Texture Initialization State.
             * 
             *  This value must be true in order to use the texture.  If any
             *  other value is set, reconfiguration must take place.
             */
            bool init_success = true;

            /*!
             *  Texture Buffer.
             */
            //unsigned char* tex_buffer = nullptr;
            /// Texture width
            //int dimx = 0;
            /// Texture height
            //int dimy = 0;

            /*!
             *  Render a character to the internal buffer.
             * 
             *  @return the character advance vector.
             */
            FT_Vector render_char(char32_t glyph, int x, int y);

        public:

            /*!
             *  Construct a new TextTexture.
             */
            TextTexture(se::Engine* engine, const char* name);

            /// Texture Destructor
            ~TextTexture();

            /*!
             *  Set the font face.
             * 
             *  This method will set the font.  If the font can not be loaded,
             *  this method will return false.
             */
            bool set_font(std::string font_name);

            /*!
             *  Set the font size.
             * 
             *  This method will set the font's height in pixels.
             */
            bool set_size(int height);

            /*!
             *  Set the texture size.
             */
            void set_texturesize(int dimx, int dimy);

            /*!
             *  Update the text.
             * 
             *  This method will regenerate the texture to display the given
             *  text.
             * 
             *  @param text Text to display on the texture.
             */
            void update_text(std::string text);

    };

}

#endif