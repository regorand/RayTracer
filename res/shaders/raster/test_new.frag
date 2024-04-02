#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
flat in int fragMaterialId;

#include "res/shaders/material/material.glsl"

layout (std430, binding = 3) buffer materialBuffer
{
    BufferMaterial matData[];
};

out vec4 outColor;

void main() {
    const vec3 UP = vec3(0, 1, 0);
    float normDot = dot(normalize(fragNormal), UP);
    float val = (normDot + 1) / 2;

    vec4 color = vec4(1);
    if (fragMaterialId != -1) {
        BufferMaterial mat = matData[fragMaterialId];
        color = mat.albedo;
    }

    outColor = color * val;
}