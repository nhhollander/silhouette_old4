/*!
 *  @file include/util/loadableResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _UTIL_LOADABLERESOURCE_H_
#define _UTIL_LOADABLERESOURCE_H_

#include <mutex>

namespace util {

    /// Resource state
    enum class LoadableResourceState {
        NOT_LOADED,
        LOADING,
        LOADED,
        ERROR,
        CHILD_ERROR
    };

    const char* loadable_resource_state_name(LoadableResourceState state);

    /*!
     *  Loadable Resource.
     * 
     *  Loadable resources can be loaded into and out of memory as required by
     *  the application.  For example, a texture which is not currently in use
     *  may be removed from the CPU and GPU's memory in order to reduce system
     *  resource usage.
     */
    class LoadableResource {

        private:

            /// Resource user counter
            int resource_user_counter = 0;

            /// Resource user counter mutex
            std::mutex resource_user_counter_mutex;

        protected:

            /// Resource state
            LoadableResourceState resource_state =
                LoadableResourceState::NOT_LOADED;

            /*!
             *  Load the resource.
             * 
             *  This method is invoked when the resource's active user counter
             *  moves from zero to one.
             */
            virtual void load_() = 0;

            /*!
             *  Unload the resource.
             * 
             *  This method is invoked when the resource's active user counter
             *  moves from one to zero.
             */
            virtual void unload_() = 0;

        public:

            /// Increment the user counter
            void increment_resource_user_counter();

            /// Decrement the user counter
            void decrement_resource_user_counter();

            /// Get the resource stat
            LoadableResourceState get_resource_state();

            /// Reload resource
            void reload();


    };

}

#endif