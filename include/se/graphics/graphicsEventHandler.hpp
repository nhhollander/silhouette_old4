/*!
 *  @file include/se/graphics/graphicsEventHandler.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GRAPHICSEVENTHANDLER_H_
#define _SE_GRAPHICS_GRAPHICSEVENTHANDLER_H_

#include "se/fwd.hpp"

#include <SDL2/SDL.h>

namespace se::graphics {

    /*!
     *  Graphics Event Management Class.
     * 
     *  This class takes care of core events that need to be handled by the
     *  graphics subsystem.  They exist in this class because I like it when
     *  things are absurdly modular.
     */
    class GraphicsEventHandler {

        public:

            /*!
             *  Create a new graphics event manager.
             * 
             *  @param controller   Parent controller that is being controlled.
             */
            GraphicsEventHandler(GraphicsController* controller);

        private:

            /*!
             *  Parent Graphics Controller.
             */
            GraphicsController* parent;

            /*!
             *  Window Resize Handler.
             * 
             *  Takes care of changes to the dimensions of the window, and makes
             *  sure that the important parts of the program are notificed of
             *  the change in a timely manner.
             */
            void window_resize(SDL_Event event);
    };

}

#endif