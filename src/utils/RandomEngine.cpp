
#include "RandomEngine.h"

RandomEngine::RandomEngine()
    :   mMt(std::mt19937(time(nullptr))),
        mDistribution(std::uniform_real_distribution<>(0.0, 1.0)),
        mUIntDistribution(0)
{ }

float RandomEngine::sampleFloat() {
    return mDistribution(mMt);
}

glm::vec2 RandomEngine::sampleVec2() {
    return {sampleFloat(), sampleFloat()};
}

glm::vec3 RandomEngine::sampleVec3() {
    return {sampleFloat(), sampleFloat(), sampleFloat()};
}

unsigned int RandomEngine::sampleUint() {
    return mUIntDistribution(mMt);
}
