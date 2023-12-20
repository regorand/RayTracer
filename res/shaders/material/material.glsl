struct MaterialTMP {
    vec3 albedo;
    float roughness;
    float metallic;
    // TODO
    // transmittance factor ?
    // IOR

    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material
    // sample metallic value to decide if to evalutate metallic or dielectric bsdf ?
    // -> need to consider this in pdf -> is that just a constant factor ?
    // need sheen layer for colorless highlights ?
    // metallic seems to have higher prio than transmittance of dielectric
    // -> look at principled bsdf shader example!
};