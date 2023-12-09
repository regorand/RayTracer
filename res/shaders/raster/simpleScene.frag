#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragColor;

uniform uint emissive;

out vec4 outColor;

void main() {
    float ambientFac = 0.5;
    float dotFactor = (dot(fragNormal, vec3(0, 1, 0)) + 1) / 2;
    if (emissive == 0) {
        outColor = vec4(fragColor * (ambientFac + (1 - ambientFac) * dotFactor), 1);
    } else {
        outColor = vec4(1, 0, 0, 1);
    }
}