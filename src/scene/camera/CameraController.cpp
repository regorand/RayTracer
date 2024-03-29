#include "CameraController.h"
#include "imgui.h"

void CameraController::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Camera Controls")) {
        ImGui::Text("Camera Controls Go here");
    }
}

void CameraController::addCamera(Camera camera) {
    cameras.push_back(camera);
}

void CameraController::addCameras(std::vector<Camera> &newCameras) {
    if (!newCameras.empty()) {
        cameras.insert(cameras.end(), newCameras.begin(), newCameras.end());
    }
}
