/*!
 *  @file include/se/graphics/graphicsController.hpp
 * 
 *  Definitions for the OpenGL graphics subsystem.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GRAPHICSCONTROLLER_H_
#define _SE_GRAPHICS_GRAPHICSCONTROLLER_H_

#include "se/fwd.hpp"

namespace se::graphics {

    /*!
    *  Graphics Controller.
    *
    *  The graphics controller is responsible for initializing OpenGL and
    *  managing the render pipeline.
    */
    class GraphicsController {

        private:

            /*!
             *  Parent Engine Instance
             */
            se::Engine* engine = nullptr;

        public:

            /*
             *  Construct a new graphics controller.
             */
            GraphicsController(se::Engine* engine);
    };

}

#endif