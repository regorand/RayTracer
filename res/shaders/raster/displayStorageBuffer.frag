#version 430 core

in vec2 uv;

layout (std430, binding = 0) buffer buf
{
    vec4 data[];
};

uniform int width;
uniform int totalBufferSize;

out vec4 outColor;

void main() {
    ivec2 xyCoords = ivec2(gl_FragCoord.xy);
    int coord = xyCoords.y * width + xyCoords.x;

    if (coord >= totalBufferSize) {
        discard;
    }

    outColor = vec4(data[coord].xyz, 1);
}