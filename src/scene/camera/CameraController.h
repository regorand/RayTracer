#pragma once


#include "gui/ImGuiSidebarTab.h"
#include "renderers/Rasterizer.h"
#include "renderers/PathTracer.h"

class CameraController : public ImGuiSidebarTab {

private:
    PathTracer &pt;
    Rasterizer &rasterizer;

public:
    CameraController(PathTracer &pt, Rasterizer &rasterizer);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;



};
