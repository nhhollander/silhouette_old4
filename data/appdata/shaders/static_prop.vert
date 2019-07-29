/*!
 *  Static Prop Vertex Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

layout(location = LOC_IN_VERTEX) in vec3 vertex_position;

layout(location = LOC_IN_MVP) uniform mat4 mvp_matrix;

void main() {
    
    gl_Position = mvp_matrix * vec4(vertex_position, 1.0);

}