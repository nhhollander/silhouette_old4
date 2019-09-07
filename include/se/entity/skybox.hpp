/*!
 *  @file include/se/entity/skybox.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENTITY_SKYBOX_H_
#define _SE_ENTITY_SKYBOX_H_

#include "se/entity.hpp"
#include "se/fwd.hpp"

namespace se::entity {

    /*!
     *  Skybox.
     * 
     *  The skybox is a very large cube or circle that's positioned to
     *  completely enclose the viewer.  The skybox is rendered to a separate
     *  texture because reasons.
     */
    class Skybox : public se::Entity {

        private:

            /// Program used for rendering the skybox.
            se::graphics::ShaderProgram* shader_program;

            /// Geometry of the skybox (either a cube or circle).
            se::graphics::Geometry* geometry;

            /// Skybox texture.
            se::graphics::Texture* texture;

            /// Parent Engine.
            se::Engine* engine;

            /*!
             *  Skybox Texture Name.
             * 
             *  This value is primarily used for debugging.
             */
            const char* texture_name;

        public:

            /*!
             *  Construct a new skybox.
             * 
             *  @param texture  Texture to apply to the skybox
             */
            Skybox(se::Engine* engine, const char* texture);

            /// Destroy a skybox
            ~Skybox();

            /// Render the skybox
            void render(glm::mat4 camera_matrix);

            /// Skyboxes are renderable
            bool is_renderable() { return true; }
            /// Skyboxes are not tickable
            bool is_tickable() { return false; }

            const char* get_type() { return "skybox"; }

    };

}

#endif