#pragma once

#include "OfflineRenderer.h"

class MCIntegrator : public OfflineRenderer {
    bool doBrdf = true;
    bool doNee = true;

    int maxBounces = 4;
    bool doRR = false;
    float alphaRR = 0.8;

public:
    MCIntegrator();

    void renderSample() override;

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

protected:
    void bindUniforms() override;

};
