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
layout(location = LOC_TEX_SCR_COLOR) uniform sampler2D input_color;
layout(location = LOC_TEX_SCR_BG) uniform sampler2D input_bg;
layout(location = LOC_TEX_SCR_DEPTH) uniform sampler2D input_depth;
layout(location = LOC_DIMX) uniform int dimx;
layout(location = LOC_DIMY) uniform int dimy;
layout(location = LOC_MSAA_LEVEL) uniform int msaa_lev;
layout(location = LOC_CAM_NEAR) uniform float cam_near;
layout(location = LOC_CAM_FAR) uniform float cam_far;

out vec3 color;

vec4 simple_blur(sampler2D image, vec2 uv, float radius, float stepsize) {
    vec4 accumulator = vec4(0.0);
    int counter = 0;
    for(float x = -1.0; x <= 1.0; x += stepsize) {
        for(float y = -1.0; y <= 1.0; y+= stepsize) {
            vec2 offset = vec2(x,y)*radius;
            accumulator += texture(image, uv + offset);
            counter += 1;
        }
    }
    return accumulator / float(counter);
}

float vignette(vec2 uv, float scale) {
    float dx = (uv.x - 0.5) * 2.0;
    float dy = (uv.y - 0.5) * 2.0;
    float dist = sqrt(dx*dx + dy*dy) / 1.41421356237;
    return pow(dist,scale);
}

void main() {
    
    vec2 uv_real = (uv + 1.0) / 2.0;

    float blur_radius = vignette(uv_real, 2.1);
    float vign = 1.0 - (vignette(uv_real, 4.0) * 0.5);
    

    vec4 blur_image = simple_blur(input_color, uv_real, 0.01 * blur_radius, 0.25);

    color = blur_image.xyz * vign;


}