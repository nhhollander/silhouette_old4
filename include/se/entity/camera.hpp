/*!
 *  @file include/se/entity/camera.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENTITY_CAMERA_H_
#define _SE_ENTITY_CAMERA_H_

#include "se/entity.hpp"

#include "glm/mat4x4.hpp"

namespace se::entity {

    class Camera: public se::Entity {

        public:

            bool is_renderable() { return false; }
            bool is_tickable() { return false; }

            /*!
             *  Get Camera Matrix.
             * 
             *  Returns the transformation matrix for the camera.  This accounts
             *  for the position, rotation, and scale of the camera.
             * 
             *  If you want me to explain how this works you're gonna need to
             *  buy me some alcohol first.
             */
            glm::mat4 get_camera_matrix();
    };

}

#endif