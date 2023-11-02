
#include "UniformDistribution.h"

UniformDistribution::UniformDistribution()
    :   mMt(std::mt19937(time(nullptr))),
        mDistribution(std::uniform_real_distribution<>(0.0, 1.0))
{ }

float UniformDistribution::sampleFloat() {
    return mDistribution(mMt);
}

glm::vec2 UniformDistribution::sampleVec2() {
    return {sampleFloat(), sampleFloat()};
}

glm::vec3 UniformDistribution::sampleVec3() {
    return {sampleFloat(), sampleFloat(), sampleFloat()};
}
