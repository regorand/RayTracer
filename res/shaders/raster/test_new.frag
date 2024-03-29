#version 430 core

in vec3 fragPos;
in vec3 fragNormal;
// flat in int fragMaterialId;

out vec4 outColor;

void main() {
    const vec3 UP = vec3(0, 1, 0);
    float normDot = dot(normalize(fragNormal), UP);
    float val = (normDot + 1) / 2;

    outColor = vec4(vec3(val), 1);
}