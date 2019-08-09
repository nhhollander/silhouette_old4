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
    // Calculate the fresnel highlight
    float fhv = 1.0 - dot(normal,vec3(0.0, -1.0, 0.0));
    fhv = pow(fhv, 4) * 0.5;
    vec3 fresnel_highlight = vec3(fhv,fhv,fhv);
    // Apply the fresnel highlight
    color = color + fresnel_highlight;
}