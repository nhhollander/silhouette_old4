/*!
 *  Static Prop Fragment Shader
 * 
 *  Copyright 2019 Nicholas Hollander <nhhollander@wpi.edu>
 * 
 *  Licensed under the MIT license (see LICENSE for the complete text)
 */

out vec3 color;

in vec3 fragment_color;

void main() {
    color = fragment_color;
}