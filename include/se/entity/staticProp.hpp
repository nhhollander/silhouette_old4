/*!
 *  @file include/se/entity/staticProp.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENTITY_STATICPROP_H_
#define _SE_ENTITY_STATICPROP_H_

#include "se/entity.hpp"
#include "se/fwd.hpp"

namespace se::entity {

    /*!
     *  Static Prop.
     * 
     *  Static props are renderable entities within the world that have no
     *  internal logic, and unless acted upon by outside forces, do absolutely
     *  nothing.
     */
    class StaticProp : public se::Entity {

        private:

            /// Program used for rendering this prop
            se::graphics::ShaderProgram* shader_program;

            /// Geometry of this prop
            se::graphics::Geometry* geometry;

            /*!
             *  Texture to be applied to this prop.
             * 
             *  TODO: It's silly to assume that all static props will only have
             *  a single texture.  At some point in the future this will have to
             *  be replaced by a more configurable system with multiple types
             *  of textures to be applied to models.
             */
            se::graphics::Texture* texture;

            /*!
             *  Model Name.
             * 
             *  This value is primarily used for debugging.
             */
            const char* model_name;

            /*!
             *  Texture Name.
             * 
             *  This value is primarily use debugging.
             */
            const char* texture_name;

            /// Parent Engine.
            se::Engine* engine;

        public:

            /// Construct a static prop
            StaticProp(se::Engine* engine, const char* model, const char* texture);

            /// Destroy a static prop
            ~StaticProp();

            /// Render the prop
            void render();

            /// Static props are renderable
            bool is_renderable() { return true; }
            /// Static props are not tickable
            bool is_tickable() { return false; }

    };

}

#endif