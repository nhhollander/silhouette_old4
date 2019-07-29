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
#include "se/fwd.hpp"

#include "glm/mat4x4.hpp"

namespace se::entity {

    /*!
     *  Camera Entities.
     * 
     *  Cameras represent the things that take pictures of the world, usually
     *  quite a few times per second.
     * 
     *  Cameras have an adjustable field of view, as well as a minimum and
     *  maximum render distance.  Try to keep the minimum render distance
     *  relatively larte and the maximum relatively small to reduce visual
     *  glitches caused by floating point math errors.  Although extremely
     *  large maximums will allow the camera to see an extreme distance, objects
     *  that are extremely close to the camera will begin to experience strange
     *  depth buffer glitches due to the diminished z buffer resolution.
     */
    class Camera: public se::Entity {

        private:

            /// Pointer to the height value as a float
            const volatile int* height = nullptr;

            /// Pointer to the width value as a float
            const volatile int* width = nullptr;

        public:

            /// Camera Constructor
            Camera(se::Engine* engine);

            bool is_renderable() { return false; }
            bool is_tickable() { return false; }

            /// Field of View (radians)
            float fov = 1.22173; // 70 degrees

            /// Near render boundary
            float near = 0.01;

            /// Far render boundary
            float far = 500.0;

            /*!
             *  Get Camera Matrix.
             * 
             *  Returns the transformation matrix for the camera.  This accounts
             *  for the position, rotation, and scale of the camera (the VP
             *  part of the Model View Projection system for 3D graphics.)
             * 
             *  If you want me to explain how this works you're gonna need to
             *  buy me some alcohol first.
             */
            glm::mat4 get_camera_matrix();
    };

}

#endif