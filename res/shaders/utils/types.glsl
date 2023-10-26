struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Material {
    vec3 color;
    float diffuse; // 1 - diffuse is specular term
};

struct Sphere {
    vec3 center;
    float radius;
    Material mat;
};