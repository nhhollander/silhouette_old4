/*!
 *  @file src/se/entity/camera.cpp
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

#include "se/entity/camera.hpp"

#include "se/engine.hpp"

#include "util/config.hpp"
#include "util/log.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

using namespace se::entity;

Camera::Camera(se::Engine* engine) {
    this->width = engine->config->get_intp("window.dimx");
    this->height = engine->config->get_intp("window.dimy");
    this->near = engine->config->get_floatp("render.cam_near");
    this->far = engine->config->get_floatp("render.cam_far");
    this->fov = engine->config->get_floatp("render.fov");
}

Camera::~Camera() {}

#define MATRIX(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) { \
    {a,e,i,m}, \
    {b,f,j,n}, \
    {c,g,k,o}, \
    {d,h,l,p} \
}

glm::mat4 Camera::get_camera_matrix() {

    /* Okay here's a quick lesson in linear algebra.  If you have a point that
    is located somewhere in 3D space, and you want to transform that point to a
    different location in 3D space, how are you going to do it?  The answer is
    a transformation matrix.  These bad boys can contain any number of arbitrary
    transformations combined into a 4x4 matrix.  Rotations, scales,
    translations, perspective shifts, just about anything goes.  The other
    important thing is that combining transformation matrices is done from
    right to left.  I don't think any crazy mathematitian decided to make it
    like that, numbers just sort of did it on their own.  So if you want to
    rotate a point about its origin and then translate it, your math will be
    evaluated from right to left.  Adding a bit of confusion to this is that
    you can multiply the matrices in any order, so long as they all get squashed
    into the process starting at the right, and going left.  This has to do with
    the fact that matrices don't multiply the same way as numbers, but I'm not
    going to explain how that works, just understand that `a * b != b * a`
    in this crazy math hellscape we're dealing with right now. */

    /* If you want to understand what's going on here, I recommend you take a
    look at this fantastic tutorial on opengl-tutorial.org:
    http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/ */

    /* The camera object matrix will perform 4 transformations on the points in
    the world.  Keep in mind these operations are done in the opposite order
    from what is done to entities, because we're applying this operation to the
    entire world around the camera, and you know what I don't understand it well
    enough to explain to you so I'm going to stop writing comments now. */

    /* Values are inverted because we're moving the world relative to the camera
    instead of moving the camera realative to the world. */

    glm::mat4 translate = MATRIX(
        1.0, 0.0, 0.0, -this->x,
        0.0, 1.0, 0.0, -this->y,
        0.0, 0.0, 1.0, -this->z,
        0.0, 0.0, 0.0, 1.0     
    );

    /* A 90 degree offset is applied to the angle to shift the world around so
    that Z is the up direction, and by default the camera is looking directly
    along the +Y axis */
    float xcos = cos(-this->rx - 1.5708);
    float xsin = sin(-this->rx - 1.5708);
    glm::mat4 rotate_x = MATRIX(
        1.0,  0.0,   0.0, 0.0,
        0.0, xcos, -xsin, 0.0,
        0.0, xsin,  xcos, 0.0,
        0.0,  0.0,   0.0, 1.0
    );

    float ycos = cos(-this->ry);
    float ysin = sin(-this->ry);
    glm::mat4 rotate_y = MATRIX(
         ycos, 0.0, ysin, 0.0,
          0.0, 1.0,  0.0, 0.0,
        -ysin, 0.0, ycos, 0.0,
          0.0, 0.0,  0.0, 1.0
    );

    float zcos = cos(-this->rz);
    float zsin = sin(-this->rz);
    glm::mat4 rotate_z = MATRIX(
        zcos, -zsin, 0.0, 0.0,
        zsin,  zcos, 0.0, 0.0,
         0.0,   0.0, 1.0, 0.0,
         0.0,   0.0, 0.0, 1.0
    );

    glm::mat4 scale = MATRIX(
        1.0 / this->sx, 0.0, 0.0, 0.0,
        0.0, 1.0 / this->sy, 0.0, 0.0,
        0.0, 0.0, 1.0 / this->sz, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    /* I'm using the glm method here because there's basically no
    performance to be saved by hand-calculating the values like there was above.
    It might be worth a future expansion to save some cycles in not
    recalculating values that don't need to be constantly recalculated, but
    that sounds like a problem for the future. */
    float aspect = ((float) *(this->width)) / ((float) *(this->height));
    glm::mat4 projection = glm::perspective(*this->fov, aspect, *this->near, *this->far);

    /* Operations are performed from right to left, in the opposite order from
    models.  This means that the models will first be translated into position,
    then rotated about the camera (in x->y->z order), scaled, and finally
    projected into screen space. */
    return projection * scale * rotate_x * rotate_y * rotate_z * translate;

}