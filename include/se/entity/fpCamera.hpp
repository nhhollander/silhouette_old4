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
#include "se/logic/logicController.hpp"

#include <SDL2/SDL.h>

namespace se::entity {

    /*!
     *  First Person Camera.
     */
    class FPCamera : public Camera, se::logic::Tickable {

        private:

            void sdl_event_handler(SDL_Event event);

            bool key_w = false;
            bool key_s = false;
            bool key_a = false;
            bool key_d = false;

            /// Camera locked
            bool camera_locked = false;

        public:

            FPCamera(se::Engine* engine);
            ~FPCamera();

            const char* get_type() { return "first_person_camera"; }

            void tick(uint64_t clock, uint32_t cdelta);

            /// Lock the mouse to the center of the window
            void lock_mouse();

            /// Release the mouse
            void release_mouse();

    };

}

#endif