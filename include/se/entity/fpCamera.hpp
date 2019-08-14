/*!
 *  @file include/se/entity/camera.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENTITY_FPCAMERA_H_
#define _SE_ENTITY_FPCAMERA_H_

#include "se/entity/camera.hpp"
#include "se/fwd.hpp"

#include <SDL2/SDL.h>

namespace se::entity {

    /*!
     *  First Person Camera.
     */
    class FPCamera : public Camera {

        private:

            void sdl_event_handler(SDL_Event event);

        public:

            FPCamera(se::Engine* engine);

            /// Lock the mouse to the center of the window
            void lock_mouse();

            /// Release the mouse
            void release_mouse();

    };

}

#endif