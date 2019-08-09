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

out vec4 color;

void main() {
    color = texture(texture_sampler, uv);
}