
#ifndef RAYTRACER_UNIFORMDISTRIBUTION_H
#define RAYTRACER_UNIFORMDISTRIBUTION_H

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <random>
#include <ctime>

class UniformDistribution {

private:

    std::mt19937 mMt;
    std::uniform_real_distribution<> mDistribution;

public:
    UniformDistribution();

    float sampleFloat();
    glm::vec2 sampleVec2();
    glm::vec3 sampleVec3();

};


#endif //RAYTRACER_UNIFORMDISTRIBUTION_H
