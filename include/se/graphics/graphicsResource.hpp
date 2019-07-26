/*!
 *  @file include/se/graphics/graphicsResource.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GRAPHICSRESOURCE_H_
#define _SE_GRAPHICS_GRAPHICSRESOURCE_H_

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
     *  Graphics Resource Type.
     * 
     *  **Note to Developers:** When adding additional resource types here, make
     *  sure to update the corresponding entry in
     *  `graphics_resource_type_name()`.
     */
    enum class GraphicsResourceType {
        TEST_TYPE
    };

    /*!
     *  Graphics Resource Type Name
     * 
     *  Returns a textual representation of a GraphicsResourceType
     */
    const char* graphics_resource_type_name(GraphicsResourceType type);

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

            /*!
             *  Construct a new Graphics Resource.
             * 
             *  This function should only be called  */

        protected:

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

        public:

            /*!
             *  Get the state.
             * 
             *  Returns the state of this graphics resource.
             */
            GraphicsResourceState get_state();

            /*!
             *  Load the Resource.
             */
    };

}

#endif