/*!
 *  @file include/se/graphics/graphicsResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GRAPHICSRESOURCE_H_
#define _SE_GRAPHICS_GRAPHICSRESOURCE_H_

#include <cstdint>
#include <map>
#include <vector>

namespace se::graphics {

    /*!
     *  Graphics Resource State.
     * 
     *  **Note to Developers:** When adding additional states here, make sure to
     *  update the corresponding entry `in graphics_resource_state_name()`.
     */
    enum class GraphicsResourceState {
        NOT_LOADED,
        LOADING,
        ERROR,
        CHILD_ERROR,
        LOADED
    };

    /*!
     *  Graphics Resource State Name.
     * 
     *  Returns a textual representation of a GraphicsResourceState
     */
    const char* graphics_resource_state_name(GraphicsResourceState state);

    /*!
     *  Graphics Resource Base Class.
     * 
     *  Due to the inherently limted nature of many compute resources, the
     *  silhouette engine can not possibly store every asset in memory at any
     *  given time.  This class provides a set of resources for swapping
     *  memory-intensive assets out of GPU memory in order to free up space.
     */
    class GraphicsResource {

        private:

            /// Resource Cache
            static std::map<uint32_t, GraphicsResource*> resource_cache;

        protected:

            /*!
             *  Construct a new Graphics Resource.
             * 
             *  Only one instance of any specific resource should ever be
             *  constructed.
             * 
             *  Construction requires a globally unique identifier.
             */
            GraphicsResource(uint32_t unique_id);

            /*!
             *  Load the resource (internal).
             * 
             *  This function will be called when the resource is asked to be
             *  loaded.
             */
            virtual void load_() = 0;

            /*!
             *  Unload the resource (internal).
             * 
             *  This function will be called when the resource is asked to be
             *  unloaded.
             */
            virtual void unload_() = 0;

            /// Resource state
            GraphicsResourceState resource_state = GraphicsResourceState::NOT_LOADED;

            /*!
             *  Active User Counter.
             * 
             *  This value contains the number of entities that are currently
             *  using this resource.  This does not necessarily reflect the
             *  number of constructed entities that depend on this resource.
             */
            uint16_t active_users = 0;

            /*!
             *  Get Resource.
             * 
             *  Returns a pointer to an instance of a given graphics resource,
             *  or `nullptr` if it has not yet been initialized.
             * 
             *  Make sure that the `unique_id` given here is the same that was
             *  used when constructing the object.
             *
             *  @param unique_id    Unique identifier
             */
            static GraphicsResource* get_resource(uint32_t unique_id);

        public:

            /*!
             *  Get the state.
             * 
             *  Returns the state of this graphics resource.
             */
            GraphicsResourceState get_resource_state();

            /*!
             *  Increment the active user counter.
             * 
             *  If a call to this function causes the active user count to move
             *  from 0 to 1, the resource will be loaded and made available for
             *  use as quickly as possible.
             * 
             *  **Make sure to decrement the user counter when you're done with
             *  the resource**
             */
            void increment_active_users();

            /*!
             *  Decrement the active user counter.
             * 
             *  If a call to this function causes the active user count to move
             *  from 1 to 0, the resource may be unloaded.
             * 
             *  **Make sure you actually incremented the user counter before**
             */
            void decrement_active_users();

    };

}

#endif