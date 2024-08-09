#pragma once


#include "gui/ImGuiSidebarTab.h"
#include "Camera.h"

const int DEFAULT_CAMERA_INDEX = 0;

class CameraController : public ImGuiSidebarTab {
private:
    std::vector<Camera> cameras;


public:
    CameraController();

    Camera &getCameraByIndex(int index);

    void addCamera(Camera camera);
    void addCameras(std::vector<Camera> &cameras);

    void buildImguiTab(DataStore &settings, std::vector<Event> &events) override;



};
