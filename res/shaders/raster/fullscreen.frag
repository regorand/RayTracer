#version 430 core

in vec2 uv;

uniform sampler2D tex;

out vec4 outColor;

void main() {
    vec4 texValue = texture(tex, uv);
    outColor = vec4(texValue.rgb / texValue.a, 1);
}