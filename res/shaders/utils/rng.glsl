float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// some basic random building blocks from https://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
uint rand_lcg(uint rng_state) {
    rng_state = 1664525 * rng_state + 1013904223;
    return rng_state;
}

uint rand_xorshift(uint rng_state)
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_staseedte ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

// This is my own, although I am sure this has been done before
float randUniform_0_1(inout float seed) {
    uint intSeed = floatBitsToUint(seed);
    seed = uintBitsToFloat(rand_lcg(intSeed));
    // seed = uintBitsToFloat(rand_xorshift(intSeed));
    // seed = uintBitsToFloat(wang_hash(intSeed));
    return fract(seed);
}