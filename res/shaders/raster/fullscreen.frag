#version 430 core

in vec2 uv;

uniform sampler2D tex;

out vec4 outColor;

void main() {
    outColor = vec4(texture(tex, uv).rgb, 1);
}