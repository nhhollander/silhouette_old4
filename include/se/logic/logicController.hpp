/*!
 *  @file include/se/logic/logicController.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_LOGIC_LOGICCONTROLLER_H_
#define _SE_LOGIC_LOGICCONTROLLER_H_

#include "se/fwd.hpp"

#include <thread>
#include <vector>

namespace se::logic {

    /*!
     *  Tickable Generic.
     * 
     *  Tickable objects are objects that are capable of receiving a tick event.
     */
    class Tickable {

        public:

            /*!
             *  Tick Handler.
             * 
             *  This method will be called at a regular interval, and is
             *  intended to be used for processing logic events.  It is
             *  important to design this method such that it executes as quickly
             *  as possible.  Slow execution may result in severe engine lag.
             * 
             *  Keep in mind that the logic controller has a configurable time
             *  scaler, which can change the speed of passage of time within the
             *  engine.  For example, if the engine is operating at a scale 
             *  factor of 50% with a configured TPS of 1, tickables will be
             *  called every 1000 milliseconds, but `cdelta` will only be 500,
             *  and `clock` will only have advanced by 500.
             * 
             *  This method takes a `clock` parameter which contains a scaled
             *  engine time varaible, in relative milliseconds.  Because the
             *  logic controller is capable of warping time, and other tickables
             *  are capable of introducing severe lag, **you must not assume
             *  constant passage of time between ticks**.  The parameter
             *  `cdelta` contains the number of scaled milliseconds since the
             *  last tick.  Use only `clock` and `cdelta` for in-game time based
             *  calculations.
             * 
             *  @param clock    Scaled engine time.
             *  @param cdelta   Scaled time since last tick.
             */
            virtual void tick(uint64_t clock, uint32_t cdelta) = 0;

    };

    /*!
     *  Logic Controller.
     * 
     *  The logic controller is responsible for handling scheduled repeating
     *  logic events.
     */
    class LogicController {

        private:

            /// Parent Engine Instance.
            se::Engine* engine = nullptr;

            /// Logic Thread Object
            std::thread logic_thread;

            /*!
             *  Target tick time.
             * 
             *  TPS determines the number of times the engine will execute
             *  logic events per second.
             * 
             *  This variable detmines the number of nanoseconds each logic
             *  processing event should last.
             */
            uint64_t target_tick_time = 0;

            /*!
             *  Tickables.
             * 
             *  For the time being all logical entities are lumped into one
             *  giant list.  This will need to be optimized later.
             * 
             *  For the time being all tickables are lumped into one giant list.
             *  This will need to be optimized later.
             */
            std::vector<Tickable*> tickables;

            /*!
             *  Logic Thread.
             * 
             *  This method is spawned as the main body of the tick thread.  It
             *  will continue to run for as long as `se::engine::threads_run` is
             *  set to `true`.
             */
            void logic_thread_main();

            /*!
             *  Time Scale Pointer.
             * 
             *  Pointer to the time scale configuration value.
             */
            volatile double* time_scale;

            /*!
             *  Scaled Engine Time.
             * 
             *  Contains a time-scaled clock value.
             */
            uint32_t scaled_clock = 0;

        public:

            /// Construct a new logic controller
            LogicController(se::Engine* engine);

            /// Destroy this logic controller
            ~LogicController();

            /// Register a tickable
            void register_tickable(Tickable* tickable);

            /// Deregister a tickable
            void deregister_tickable(Tickable* tickable);

    };

}

#endif