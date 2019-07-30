/*!
 *  Static Prop Vertex Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

layout(location = LOC_IN_VERT) in vec3 vertex_position;
layout(location = LOC_IN_UV)   in vec2 vert_uv;
layout(location = LOC_IN_NORM) in vec3 normal;

layout(location = LOC_IN_MVP) uniform mat4 mvp_matrix;

out vec3 fragment_color;
out vec2 uv;

void main() {
    
    gl_Position = mvp_matrix * vec4(vertex_position, 1.0);

    fragment_color = normal;

    uv = vert_uv;

}