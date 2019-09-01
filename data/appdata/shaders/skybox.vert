/*!
 *  Skybox Vertex Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

// Model parameter inputs
layout(location = LOC_IN_VERT) in vec3 vert_in;
layout(location = LOC_IN_UV)   in vec2 uv_in;

// Render parameter inputs
layout(location = LOC_IN_MVP) uniform mat4 mvp_in;

// Misc. Outputs
out vec2 uv;

void main() {

    vec4 pos = vec4(vert_in, 1.0);
    gl_Position = mvp_in * pos;

    uv = uv_in;

}