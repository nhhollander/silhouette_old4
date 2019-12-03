/*!
 *  @file include/se/graphics/geometry.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GEOMETRY_H_
#define _SE_GRAPHICS_GEOMETRY_H_

#include "se/util/cacheableResource.hpp"
#include "se/util/loadableResource.hpp"

#include "se/fwd.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace se::graphics {

    /*!
     *  Geometry Class.
     */
    class Geometry : public util::CacheableResource, public util::LoadableResource {

        private:

            /// Geometry Name
            const char* name;

            /// Parent Engine
            se::Engine* engine;

            /*!
             *  Construct a new Geometry.
             * 
             *  Where possible, it is preferered to retrieve geometry from the
             *  resource cache instead of constructing duplicates.
             */
            Geometry(se::Engine* engine, const char* name);

            /// Geometry Destructor
            ~Geometry();

            /*!
             *  Raw Vertex Data.
             * 
             *  This variable may be cleared after binding.
             */
            std::vector<glm::vec3> vertex_data;
            
            /*!
             *  Raw UV Data.
             * 
             *  This variable may be cleared after binding.
             */
            std::vector<glm::vec2> uv_data;

            /*!
             *  Raw Normal Data.
             * 
             *  This variable may be cleared after binding.
             */
            std::vector<glm::vec3> normal_data;

            /// OpenGL vertex array object ID
            unsigned int gl_vertex_array_object_id = 0;

            /// OpenGL vertex buffer ID
            unsigned int gl_vertex_buffer_id = 0;

            /// OpenGL uv buffer ID
            unsigned int gl_uv_buffer_id = 0;

            /// OpenGL normal buffer ID
            unsigned int gl_normal_buffer_id = 0;

            /// Size of the vertex array
            unsigned int vertex_array_size = 0;

            /*!
             *  Bind geometry to the GPU.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void bind();

            /*!
             *  Unbind geometry from the GPU.
             * 
             *  **Warning:** This method must only be called from the graphics
             *  thread.
             */
            void unbind();

        protected:

            /// @see `util::LoadableResource::load_()`
            void load_();

            /// @see `util::LoadableResource::unload_()`
            void unload_();

            /// @see `util::CacheableResource::resource_id()`
            uint32_t resource_id();

            /// @see `util::CacheableResource::resource_name()`
            std::string resource_name();

        public:

            /*!
             *  Get an instance of a model.
             * 
             *  Attempts to load the requested geometry from the graphics
             *  resource cache, and failing that, returns a new object.
             * 
             */
            static Geometry* get_geometry(se::Engine* engine, const char* name);

            /*!
             *  Use this Geometry.
             * 
             *  **Warning:** This method must be called from the graphics
             *  thread.
             */
            void use_geometry();

    };

}

#endif