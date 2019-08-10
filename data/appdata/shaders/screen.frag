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
layout(location = LOC_TEX_(0)) uniform sampler2DMS texture_sampler;
layout(location = LOC_DIMX) uniform int dimx;
layout(location = LOC_DIMY) uniform int dimy;
layout(location = LOC_MSAA_LEVEL) uniform int msaa_lev;

out vec3 color;

void main() {
    
    // Antialiasing
    int x = int(((uv.x + 1.0) / 2.0) * dimx);
    int y = int(((uv.y + 1.0) / 2.0) * dimy);
    ivec2 coord = ivec2(x,y);
    vec3 accum = vec3(0,0,0);
    for(int i = 0; i < msaa_lev; i++) {
        accum += texelFetch(texture_sampler, coord, i).xyz;
    }
    color = accum / msaa_lev;
}