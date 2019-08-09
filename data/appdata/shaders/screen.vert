/*!
 *  Screen Output Vertex Shader.
 *
 *  This super simple shader just passes points through.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

// Inputs
layout(location = LOC_IN_VERT) in vec3 vert;

out vec2 uv;

void main() {
    gl_position = vert;
    uv = vert.xy;
}