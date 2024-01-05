float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// hash from https://www.shadertoy.com/view/WttXWX
uint triple32(uint x)
{
    x ^= x >> 17;
    x *= 0xed5ad4bbU;
    x ^= x >> 11;
    x *= 0xac4c1b51U;
    x ^= x >> 15;
    x *= 0x31848babU;
    x ^= x >> 14;
    return x;
}

// some basic random building blocks from https://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
uint rand_lcg(uint rng_state) {
    rng_state = 1664525 * rng_state + 1013904223;
    return rng_state;
}

uint rand_xorshift(uint rng_state)
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

uint wang_hash(uint seed)
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15u);
    return seed;
}

// From https://stackoverflow.com/a/17479300
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}


// This is my own, although I am sure this has been done before
float randUniform_0_1(inout uint seed) {
    // uint intSeed = floatBitsToUint(seed);
    // seed = uintBitsToFloat(rand_lcg(intSeed));
    // seed = uintBitsToFloat(rand_xorshift(intSeed));
    seed = wang_hash(seed);
    return floatConstruct(seed);
}