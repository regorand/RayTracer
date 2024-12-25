#pragma once

#include <cstdint>

struct RenderStatistics {
    uint32_t sampleCount {};

    // might be different from sampleCount since timing collection can be specifically turned on and off without reseting texture
    uint32_t timingSamplesCollected {};
    double lastMsTime {};
    double summedTime {};
    double movingAverage {};

    double alpha = 0.1;

    void reset() {
        sampleCount = 0;
        resetTimings();
    }

    void resetTimings() {
        timingSamplesCollected = 0;
        lastMsTime = 0;
        summedTime = 0;
        movingAverage = 0;
    }
};