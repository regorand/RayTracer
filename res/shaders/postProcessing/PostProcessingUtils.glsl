void GammaCorrection(inout vec3 value) {
    value = pow(value, vec3(1 / 2.2));
}

void Reinhard(inout vec3 value) {
    value = value / (value + 1);
}

void ReinhardWhiteVal(float whiteVal, inout vec3 value) {
    value = value * (1 + value / (whiteVal * whiteVal)) / (value + 1);
}
