/*!
 *  @file include/se/engine.hpp
 * 
 *  Definitions for the primary Engine class.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENGINE_H_
#define _SE_ENGINE_H_

#include "se/fwd.hpp"
#include "util/fwd.hpp"

namespace se {

    /*!
     *  Engine Class.
     *
     *  This class represents the engine.  Currently, the engine is implemented
     *  as a singleton because that's a lot easier than trying to pass an engine
     *  reference to every child object, and I could not think of an instance
     *  where multiple engines would need to exist within a single application.
     */
    class Engine {

        public:

            /*!
             *  Configuration Object.
             *
             *  All configuration settings for this instance of this engine are
             *  contained here.
             */
            util::Configuration* config = nullptr;

            /*!
             *  Graphics Controller Instance.
             */
            se::graphics::GraphicsController* graphics_controller = nullptr;

            /*
             *  Create a new Engine Instance.
             */
            Engine();

            /*
             *  Destroy an Engine
             */
            ~Engine();
            

    };

    /*!
     *  Default Engine Pointer.
     */
    se::Engine* default_engine;

}

#endif