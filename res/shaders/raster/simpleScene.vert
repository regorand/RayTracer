#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 MVP;
uniform mat4 transform;
uniform mat4 normalTransform;

out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragColor;

void main() {
    gl_Position = MVP * vec4(position, 1);
    vec4 homogPos = transform * vec4(position, 1);
    fragPos = homogPos.xyz / homogPos.w;
    fragNormal = (normalTransform * vec4(normal, 0)).xyz;
    fragColor = color;
}