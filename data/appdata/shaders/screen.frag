/*!
 *  Screen Output Fragment Shader.
 *
 *  This shader combines and renders frame buffers to the display.
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

// Inputs
in vec2 uv;
layout(location = LOC_TEX_(0)) uniform sampler2D texture_sampler;

out vec3 color;

void main() {
    vec2 new_uv = vec2((uv.x + 1.0)/2.0,(uv.y + 1.0)/2.0);
    color = texture(texture_sampler, new_uv).xyz;
}