#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in int matIndex;

uniform mat4 transform;
uniform mat4 MVP;
uniform mat4 normalTransform;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragUV;
flat out int fragMatIndex;

void main() {
    gl_Position = MVP * vec4(position, 1);
    vec4 homogPos = transform * vec4(position, 1);
    fragPos = homogPos.xyz / homogPos.w;
    fragNormal = (normalTransform * vec4(normal, 0)).xyz;
    fragUV = uv;

    // matIndex seems to read the 0x1 in the buffer and convert it to a float (int value 1065353216), need to bit convert it back
    float floatBits = intBitsToFloat(matIndex);
    fragMatIndex = int(floatBits);
}