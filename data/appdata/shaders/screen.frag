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
    // Simple Blur Effect
    vec2 new_uv = vec2((uv.x + 1.0)/2.0,(uv.y + 1.0)/2.0);
    vec3 u = texture(texture_sampler, vec2(new_uv.x,new_uv.y-0.004)).xyz;
    vec3 d = texture(texture_sampler, vec2(new_uv.x,new_uv.y+0.004)).xyz;
    vec3 l = texture(texture_sampler, vec2(new_uv.x-0.004,new_uv.y)).xyz;
    vec3 r = texture(texture_sampler, vec2(new_uv.x+0.004,new_uv.y)).xyz;
    vec3 c = texture(texture_sampler, new_uv).xyz;
    color = (u+d+l+r+c) / 5;

    // Abberation
    vec2 red_uv = vec2(((uv.x * 1.00) + 1.0) / 2.0, ((uv.y * 1.00) + 1.0) / 2.0);
    vec2 grn_uv = vec2(((uv.x * 0.995) + 1.0) / 2.0, ((uv.y * 0.995) + 1.0) / 2.0);
    vec2 blu_uv = vec2(((uv.x * 0.99) + 1.0) / 2.0, ((uv.y * 0.99) + 1.0) / 2.0);
    float red = texture(texture_sampler, red_uv).r;
    float grn = texture(texture_sampler, grn_uv).g;
    float blu = texture(texture_sampler, blu_uv).b;
    color = vec3(red,grn,blu);

    // Passthrough
    color = texture(texture_sampler, new_uv).xyz;
}