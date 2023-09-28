#version 430 core

layout(location = 0) in vec4 position;

out vec2 uv;

void main() {
    vec4[] pos = {
        vec4(-1.0, -1.0, 0, 1),
        vec4( 1.0, -1.0, 0, 1),
        vec4(-1.0,  1.0, 0, 1),
        vec4( 1.0,  1.0, 0, 1)
    };

    vec2[] uvs = {
        vec2(0, 0),
        vec2(1.0, 0),
        vec2(0, 1.0),
        vec2(1.0, 1.0)
    };

    gl_Position = pos[gl_VertexID].xyzw;
    uv = uvs[gl_VertexID].xy;
}