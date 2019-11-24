/*!
 *  @file include/se/entity/sign.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENTITY_SIGN_H_
#define _SE_ENTITY_SIGN_H_

#include "se/entity.hpp"
#include "se/fwd.hpp"

#include <string>

namespace se::entity {

    /*!
     *  Sign.
     * 
     *  Signs have text on them.
     */
    class Sign : public se::Entity {

        private:

            /// Program used for rendering this prop
            se::graphics::ShaderProgram* shader_program;

            /// Sign Geometry
            se::graphics::Geometry* geometry;

            /// Sign texture, generated
            se::graphics::TextTexture* texture;

            /*!
             *  Model Name.
             * 
             *  This value is primarily used for debugging.
             */
            const char* model_name;
            /// Parent Engine.
            se::Engine* engine;

        public:

            /// Construct a sign
            Sign(se::Engine* engine, const char* model);

            /// Destroy a sign
            ~Sign();

            /// Render the prop
            void render(glm::mat4 camera_matrix);

            /// Signs are renderable
            bool is_renderable() { return true; }
            /// Signs are not tickable
            bool is_tickable() { return false; }

            const char* get_type() { return "sign"; }

            void set_text(std::string text);

    };

}

#endif