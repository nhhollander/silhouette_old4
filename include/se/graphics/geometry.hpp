/*!
 *  @file include/se/graphics/geometry.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_GRAPHICS_GEOMETRY_H_
#define _SE_GRAPHICS_GEOMETRY_H_

#include "se/graphics/graphicsResource.hpp"

#include "se/fwd.hpp"

namespace se::graphics {

    /*!
     *  Geometry Class.
     */
    class Geometry : public GraphicsResource {

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
             */
            float* raw_vertex_data = nullptr;

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

            void load_();

            void unload_();

        public:

            static Geometry* get_geometry(se::Engine* engine, const char* name);

    };

}

#endif