/*!
 *  Static Prop Fragment Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

out vec3 color;

in vec3 fragment_color;
in vec2 uv;

layout(location = LOC_TEX_(0)) uniform sampler2D texture_sampler;

void main() {
    color = texture(texture_sampler, uv).rgb;
}