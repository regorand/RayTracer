#include "CameraController.h"
#include "imgui.h"

CameraController::CameraController(PathTracer &pt, Rasterizer &rasterizer)
    : pt(pt), rasterizer(rasterizer)
{}

void CameraController::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Camera Controls")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        if (ImGui::Button("Reset debug camera", buttonSize)) {
            rasterizer.getSceneRef().setCamera(pt.getSceneRef().getCamera());
        }

        if (ImGui::Button("Pathtrace camera to debug camera", buttonSize)) {
            pt.getSceneRef().setCamera(rasterizer.getSceneRef().getCamera());
        }
    }
}
