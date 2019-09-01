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
layout(location = LOC_TEX_SCR_COLOR) uniform sampler2DMS input_color;
layout(location = LOC_TEX_SCR_BG) uniform sampler2DMS input_bg;
layout(location = LOC_TEX_SCR_DEPTH) uniform sampler2DMS input_depth;
layout(location = LOC_DIMX) uniform int dimx;
layout(location = LOC_DIMY) uniform int dimy;
layout(location = LOC_MSAA_LEVEL) uniform int msaa_lev;
layout(location = LOC_CAM_NEAR) uniform float cam_near;
layout(location = LOC_CAM_FAR) uniform float cam_far;

out vec3 color;

float linearDepth(float depthSample);

void main() {
    
    // Antialiasing
    int x = int(((uv.x + 1.0) / 2.0) * dimx);
    int y = int(((uv.y + 1.0) / 2.0) * dimy);
    ivec2 coord = ivec2(x,y);
    vec3 accum = vec3(0,0,0);
    for(int i = 0; i < msaa_lev; i++) {
        /* Calculate fog density.  This should involve depth buffer
        linearization, but I have been completely unable to figure out how to
        do that.  It turns out that raising the base value to the power of
        twenty-thousand happens to produce a good visual effect, even if it
        isn't technically "correct" */

        float fog_depth = texelFetch(input_depth, coord, i).r;
        if(fog_depth == 1) {
            accum += texelFetch(input_bg, coord, i).rgb;
            //accum += vec3(1.0, 0.0, 0.0);
        } else {

            float fog_val = pow(fog_depth, 10000);
            vec3 fog_color = vec3(fog_val, fog_val, fog_val);

            vec3 raw_color = texelFetch(input_color, coord, i).rgb;

            accum += fog_color;
            accum += raw_color;
        }
    }

    color = accum / msaa_lev;

}