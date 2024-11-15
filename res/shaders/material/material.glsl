struct Material {
    vec4 albedo;
    vec4 emission;
    vec3 specular;
    float roughness;
    float metallic;
    float transparency;
    float IOR;
    float dummy2;
};

bool isEmissive(Material mat) {
    return mat.emission.x > 1e-2 || mat.emission.y > 1e-2 || mat.emission.z > 1e-2;
}