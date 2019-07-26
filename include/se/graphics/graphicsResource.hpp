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

            /// Add dependent resource
            void add_resource_dependent(GraphicsResource* resource);

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
            virtual void load_();

            /*!
             *  Unload the resource (internal).
             * 
             *  This function will be called when the resource is asked to be
             *  unloaded.
             */
            virtual void unload_();

            /// Resource state
            GraphicsResourceState resource_state = GraphicsResourceState::NOT_LOADED;

            /*!
             *  Resource Dependencies.
             * 
             *  Resources that this resource depends on being loaded in order
             *  to be used.
             */
            std::vector<GraphicsResource*> resource_dependencies;

            /*!
             *  Resource Dependents.
             * 
             *  Resources that depend on this resource being loaded in order to
             *  be used.
             */
            std::vector<GraphicsResource*> resource_dependents;

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
             *  Add a Dependency.
             * 
             *  Automatically registers this resource as a depdent of the
             *  specified dependency.
             */
            void add_resource_dependency(GraphicsResource* resource);

            /*!
             *  Unloadable.
             * 
             *  Checks if all dependent resources have been unloaded.
             */
            bool resource_unloadable();
    };

}

#endif