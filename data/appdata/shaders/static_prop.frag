/*!
 *  Static Prop Fragment Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

layout(location = 0) out vec3 color;

in vec2 uv;
in vec3 frag_pos;
in vec3 normal;

layout(location = LOC_TEX_(0)) uniform sampler2D texture_sampler;

void main() {
    // Sample the texture
    color = texture(texture_sampler, uv).rgb;

    // Calculate scene light position
    float light = dot(normal, normalize(vec3(-1,1,1)));
    if(light < 0) light = 0;

    // Ensure minimum lighting is available
    float brightness = (light * .8) + .2;

    color = color * brightness;

}