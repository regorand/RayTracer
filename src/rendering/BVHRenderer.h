#pragma once

#include "OfflineRenderer.h"

/*
 * Visualizes the BVH of the scene
 */
class BVHRenderer : public OfflineRenderer {


public:
    BVHRenderer();

    void renderSample() override;

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;

protected:
    void bindUniforms() override;

};
