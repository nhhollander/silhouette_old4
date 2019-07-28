/*!
 *  @file include/se/graphics/renderable.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_RENDERABLE_H_
#define _SE_GRAPHICS_RENDERABLE_H_

namespace se::graphics {

    class Renderable {

        public:

            virtual void render() = 0;

    };

}

#endif