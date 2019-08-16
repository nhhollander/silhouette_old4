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
        // Resolve depth
        float raw_depth = texelFetch(input_depth, coord, i).r;
        //float depth = linearDepth(raw_depth);
        float depth = pow(raw_depth, 200);
        //depth = -1.0;
        accum += vec3(depth, depth, depth);
        //accum += texelFetch(input_color, coord, i).xyz;
    }
    color = accum / msaa_lev;
    //color = color * color * color * color * color;
}

float linearDepth(float depthSample)
{
    depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * cam_near * cam_far / (cam_far + cam_near - depthSample * (cam_far - cam_near));
    return zLinear;
}