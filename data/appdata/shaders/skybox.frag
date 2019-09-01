/*!
 *  Skybox Fragment Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

layout(location = LOC_OUT_BG) out vec3 color;

in vec2 uv;

layout(location = LOC_TEX_(0)) uniform sampler2D texture_sampler;

void main() {

    // Sample the skybox texture
    color = texture(texture_sampler, uv).rgb;

}