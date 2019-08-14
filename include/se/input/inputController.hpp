/*!
 *  @file include/se/input/inputController.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_INPUT_INPUTCONTROLLER_H_
#define _SE_INPUT_INPUTCONTROLLER_H_

#include "se/fwd.hpp"

#include <thread>
#include <SDL2/SDL.h>
#include <functional>

namespace se::input {

    /*!
     *  Input Handler.
     * 
     *  Represents a function to be called in the event that a certain event is
     *  triggered.
     */
    typedef std::function<void(SDL_Event)> InputHandler;

    /*!
     *  Input Controller.
     * 
     *  The input controller is responsible for managing user input from
     *  whatever sources it decides to come from.
     */
    class InputController {

        private:

            /*!
             *  Parent Engine Instance.
             */
            se::Engine* engine = nullptr;

            /*!
             *  Input Thread Object.
             */
            std::thread input_thread;

            /*!
             *  Input Handlers.
             * 
             *  For the time being all input handlers are lumped in to one giant
             *  list.  This will need to be optimized later.
             */
            std::vector<InputHandler> handlers;

            /*!
             *  Input Thread.
             * 
             *  This method is spawned as the main body of the input thread.  It
             *  will continue to run for as long as `se::engine::threads_run` is
             *  set to true.
             */
            void input_thread_main();

            /*!
             *  Quit Game.
             * 
             *  This is a debug handler that should be removed once a better
             *  system is put in place.
             */
            void quit_handler(SDL_Event event);
    
        public:

            /// Construct a new input controller.
            InputController(se::Engine* engine);

            /// Destroy this input controller
            ~InputController();

            /// Register an input handler
            void register_handler(InputHandler handler);

            /// Deregister an input handler
            void deregister_handler(InputHandler handler);


    };

}

#endif