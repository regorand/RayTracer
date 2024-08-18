#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
flat in int fragMatIndex;

#include "res/shaders/material/material.glsl"

layout (std430, binding = 3) buffer materialBuffer
{
    Material matData[];
};

out vec4 outColor;

void main() {
    const vec3 UP = vec3(0, 1, 0);
    float normDot = dot(normalize(fragNormal), UP);
    float val = (normDot + 1) / 2;

    vec4 color = vec4(1);
    if (fragMatIndex != -1) {
        Material mat = matData[fragMatIndex];
        color = mat.albedo;
        color.a = 1;
    }

    outColor = color * val;
}