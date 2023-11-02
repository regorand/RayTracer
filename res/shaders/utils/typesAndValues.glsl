const float PI = 3.14159;

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 color;
    bool diffuse; // 1 - diffuse is specular term
    bool emissive;
    bool transmissive;
    float IOR;
};

struct RayHit {
    float dist;
    vec3 position;
    vec3 normal;
    Material mat;
    bool enteringMat;
};

struct Sphere {
    vec3 center;
    float radius;
    Material mat;
};

struct Triangle {
    vec3 P0;
    vec3 P1;
    vec3 P2;
    Material mat;
};