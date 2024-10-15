#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 VP;

out vec3 fragPos;

void main() {
    gl_Position = VP * vec4(position, 1);
}