#version 430 core

in vec3 fragPos;

out vec4 outColor;

void main() {
    outColor = vec4(vec3(gl_FragCoord.z * 2 - 1), 1);
}