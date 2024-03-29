#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
// flat in int fragMaterialId;

out vec4 outColor;

void main() {
    outColor = vec4(fragPos, 1);
}