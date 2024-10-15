#version 430 core

in vec3 fragPos;

out vec4 outColor;

uniform vec3 displayColor;

void main() {
    if (length(displayColor) < 1e-2) {
        outColor = vec4(1);
    } else {
        outColor = vec4(displayColor, 1);
    }
}