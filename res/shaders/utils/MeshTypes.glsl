struct Vertex {
    vec4 pos;
    vec4 normal;
    vec4 uv;
};

struct Material {
    vec4 color;
    // vec4 boolsSpecTransmissiveEmissive; // specular if x comp is 1, transmissive if y comp is 1
};

struct Triangle {
    Vertex V0;
    Vertex V1;
    Vertex V2;

    Material mat;
};

struct MeshObject {
    uint firstTriangleIndex;
    uint triangleCount;
    uint emissive; // 1 if emissive, 0 else
};