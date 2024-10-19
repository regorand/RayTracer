vec3 schlickFresnel(vec3 i, vec3 N, vec3 F0) {
    float cos_theta = max(dot(i, N), 0.0f);

    return F0 + (1 - F0) * pow(1 - cos_theta, 5.0f);
}

float fresnelEquation(float cos_theta, float n1, float n2) {
    float sin_theta = sqrt(1 - cos_theta * cos_theta);

    float k = 1 - pow((n1 / n2 * sin_theta), 2);

    if (k < 0) return 1.0f;

    float sqrt_k = sqrt(k);
    float num_S = n1 * cos_theta - n2 * sqrt_k;
    float denom_S = n1 * cos_theta + n2 * sqrt_k;

    float tmpR = num_S / denom_S;
    float R_s = tmpR * tmpR;

    float num_T = n1 * sqrt_k - n2 * cos_theta;
    float denom_T = n1 * sqrt_k + n2 * cos_theta;

    tmpR = num_T / denom_T;
    float R_t = tmpR * tmpR;

    return (R_s + R_t) / 2;
}