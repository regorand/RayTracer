struct Material {
    vec4 albedo;
    vec4 emission;
    float roughness;
    float metallic;
    float transparency;
    float IOR;
};

bool isEmissive(Material mat) {
    return mat.emission.x > 1e-2 || mat.emission.y > 1e-2 || mat.emission.z > 1e-2;
}