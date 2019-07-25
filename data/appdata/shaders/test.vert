#version 140
in vec2 inpos;

void main() {
    gl_Position = vec4(inpos.x, inpos.y, 0, 1);
}