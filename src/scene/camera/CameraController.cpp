#include "CameraController.h"
#include "imgui.h"

CameraController::CameraController() {
    cameras.push_back(Camera::getDefaultCamera());
}

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

Camera &CameraController::getCameraByIndex(int index) {
    if (cameras.empty()) {
        throw std::runtime_error("Camera vector in CameraController is empty, this should never have happened");
    }
    return cameras[index];
}
