#pragma once


#include "gui/ImGuiSidebarTab.h"
#include "renderers/Rasterizer.h"
#include "renderers/PathTracer.h"

class CameraController : public ImGuiSidebarTab {

private:
    std::vector<Camera> cameras;


public:

    void addCamera(Camera camera);
    void addCameras(std::vector<Camera> &cameras);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;



};
