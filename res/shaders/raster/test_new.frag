#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
flat in int fragMatIndex;

// in bool gl_FrontFacing;

#include "res/shaders/material/material.glsl"

layout (std430, binding = 3) buffer materialBuffer
{
    BufferMaterial matData[];
};

out vec4 outColor;

void main() {
    vec3 N = fragNormal;
    if (!gl_FrontFacing) {
        N *= -1;
    }

    const vec3 UP = vec3(0, 1, 0);
    float normDot = dot(normalize(N), UP);
    float val = (normDot + 3) / 4;

    vec4 color = vec4(1);
    if (fragMatIndex != -1) {
        BufferMaterial mat = matData[fragMatIndex];
        color = mat.albedo;
        color.a = 1;
    }

    outColor = color * val;
}