/*!
 *  Static Prop Vertex Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

// Model parameter inputs
layout(location = LOC_IN_VERT) in vec3 vert_in;
layout(location = LOC_IN_UV)   in vec2 uv_in;
layout(location = LOC_IN_NORM) in vec3 normal_in;

// Render parameter inputs
layout(location = LOC_IN_MVP) uniform mat4 mvp_in;
layout(location = LOC_IN_MODEL_MAT) uniform mat4 model_in;

// Misc. Outputs
out vec2 uv;
out vec3 frag_pos;
out vec3 normal;

void main() {

    vec4 pos = vec4(vert_in, 1.0);
    gl_Position = mvp_in * pos;
    frag_pos = (model_in * pos).xyz;

    uv = uv_in;
    normal = mat3(model_in) * normal_in;

}