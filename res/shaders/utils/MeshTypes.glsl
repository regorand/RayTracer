struct Vertex {
    vec4 pos;
    vec4 normal;
    vec4 uv;
};

struct Material {
    vec4 color;
};

struct Triangle {
    Vertex V0;
    Vertex V1;
    Vertex V2;

    Material mat;
};