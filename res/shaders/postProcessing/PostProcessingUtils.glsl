void applyGammaCorrection(inout vec3 value) {
    value = pow(value, vec3(1 / 2.2));
}

void applyReinhardToneMapping(inout vec3 value) {
    value = value / (value + 1);
}