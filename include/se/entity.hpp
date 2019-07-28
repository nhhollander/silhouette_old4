/*!
 *  @file include/se/entity.hpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#ifndef _SE_ENTITY_H_
#define _SE_ENTITY_H_

namespace se {

    /*!
     *  Entity Base Class.
     * 
     *  This class serves as the foundational type for anything that exists 
     *  within the 3D space of the engine.
     */
    class Entity {

        public:

            /// X position of this entity (meters)
            float x = 0.0;
            /// Y position of this entity (meters)
            float y = 0.0;
            /// Z position of this entity (meters)
            float z = 0.0;

            /// Rotation about the x axis (radians)
            float rx = 0.0;
            /// Rotation about the y axis (radians)
            float ry = 0.0;
            /// Rotation about the z axis (radians)
            float rz = 0.0;

            /// Scale along the x axis (multiplier)
            float sx = 1.0;
            /// Scale along the y axis (multiplier)
            float sy = 1.0;
            /// Scale along the z axis (multiplier)
            float sz = 1.0;

            /*!
             *  Entity is Renderable.
             * 
             *  This method should return true only if this entity is capable of
             *  being rendered, and implements the `render()` method.
             */
            virtual bool is_renderable() = 0;

            /*!
             *  Render.
             * 
             *  This method will only be called for renderable entities as
             *  determined by `is_renderable()`.
             */
            virtual void render();

            /*!
             *  Entity is Tickable.
             * 
             *  This method should return true only if this entity is capable of
             *  processing logic ticks, and implements the `tick()` method.
             */
            virtual bool is_tickable() = 0;

            /*!
             *  Tick.
             * 
             *  This method will only be called for tickable entities as
             *  determined by `is_tickable()`.
             */
            virtual void tick();
    };

}

#endif