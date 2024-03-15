#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragColor;

layout (location = 0) out vec4 outPos;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outColor;

void main() {
    ivec2 xyCoords = ivec2(gl_FragCoord.xy);

    outPos = vec4(fragPos, 1);
    outNormal = vec4(fragNormal, 1);
    outColor = vec4(fragColor, 1);
}