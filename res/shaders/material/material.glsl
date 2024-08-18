struct Material {
    vec4 albedo;
    vec4 emissionColor;
    float roughness;
    float metallic;
    float transparency;
    float emission;
    // TODO
    // transmittance factor ?
    // IORMaterial

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material
    // sample metallic value to decide if to evalutate metallic or dielectric bsdf ?
    // -> need to consider this in pdf -> is that just a constant factor ?
    // need sheen layer for colorless highlights ?
    // metallic seems to have higher prio than transmittance of dielectric
    // -> look at principled bsdf shader example!
};

struct BufferMaterial {
    vec4 albedo;
    vec4 emission;
    float roughness;
    float metallic;
    float transparency;
    float IOR;
};