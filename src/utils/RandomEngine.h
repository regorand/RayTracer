
#ifndef RAYTRACER_RANDOMENGINE_H
#define RAYTRACER_RANDOMENGINE_H

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <random>
#include <ctime>

class RandomEngine {

private:

    std::mt19937 mMt;
    std::uniform_real_distribution<> mDistribution;
    std::uniform_int_distribution<uint32_t> mUIntDistribution;

public:
    RandomEngine();

    float sampleFloat();
    glm::vec2 sampleVec2();
    glm::vec3 sampleVec3();

    uint32_t sampleUint();

};


#endif //RAYTRACER_RANDOMENGINE_H
