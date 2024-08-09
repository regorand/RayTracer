
#include "Scene.h"
#include "scene/scene_loading/ModelLoader.h"
#include "scene/buffers/import_scene_error.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "imgui.h"
#include "gui/ImguiUtils.h"
#include "glWrappers/shaders/ShaderUtils.hpp"
#include "SceneImporter.h"
#include "scene/camera/Camera.h"

void Scene::addCamera(Camera camera) {
    cameraController.addCamera(camera);
}

bool Scene::loadFile(std::string path) {
    SceneImporter importer(path);
    importer.import(sceneBuffers);
    // TODO I think after that its clean up time !! -> Get rid of a bunch of code
    // TODO for Path Tracing create Buffers here

    /*
    sceneBuffers.triangleBuffer = { static_cast<unsigned int>(triangles.size() * sizeof(Triangle)), GL_READ_BUFFER, triangles.data() };

    sceneBuffers.objectBuffer = { static_cast<unsigned int>(meshes->size() * sizeof(BufferMesh)),
                                  GL_READ_BUFFER,
                                  meshes->data() };

    sceneBuffers.materialBuffer = { static_cast<unsigned int>(materials.size() * sizeof(Triangle)), GL_READ_BUFFER, materials.data() };

    // TODO should make these read/write or does that only matter for GPU writes ?
    sceneBuffers.emissiveBuffer = { static_cast<unsigned int>(emissives->size() * sizeof(Triangle)), GL_READ_BUFFER, emissives->data() };
    sceneBuffers.transformationBuffer = { static_cast<unsigned int>(transforms->size() * sizeof(Triangle)),
                                          GL_READ_BUFFER,
                                          transforms->data() };
     */

    buildMaterialBuffer();

    mRasterBuffersNew.mVa.bind();

    GLsizei stride = sizeof(RasterVertex);

    mRasterBuffersNew.mAb = ArrayBuffer(sceneBuffers.vertices.data(), sizeof(RasterVertex) * sceneBuffers.vertices.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0); // pos
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) sizeof(glm::vec3)); // normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *) (2 * sizeof(glm::vec3))); // uv
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, stride, (void *) (2 * sizeof(glm::vec3) + sizeof(glm::vec2))); // material
    glEnableVertexAttribArray(3);

    mRasterBuffersNew.mIb = IndexBuffer(sceneBuffers.indices.data(), sceneBuffers.indices.size());

    shaderProgram = makeRasterizeProgram("res/shaders/raster/test_new.vert", "res/shaders/raster/test_new.frag", "material_test_raster");

    return true;
}

void Scene::render(Camera &camera) {
    glm::mat4 cameraMat = camera.getTransformation();
    glm::mat4 projectionMat = camera.getProjection();

    glm::mat4 VP = projectionMat * cameraMat;

    glEnable(GL_DEPTH_TEST);

    mRasterBuffersNew.mVa.bind();
    mRasterBuffersNew.mIb.bind();

    materialBuffer.bindToTarget(4);
    shaderProgram.bind();


    for (auto &mesh: sceneBuffers.meshes) {
        glm::mat4 model = sceneBuffers.transforms[mesh.bufferMesh.transformIndex];
        glm::mat4 MVP = VP * model;
        glm::mat4 normalTransform = glm::inverseTranspose(model);

        shaderProgram.setUniformMat4f("transform", model);
        shaderProgram.setUniformMat4f("MVP", MVP);

        shaderProgram.setUniformMat4f("normalTransform", normalTransform);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        int indexBufferIndex = mesh.indexOffset;
        int indexCount = mesh.indexCount;


        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void *) (indexBufferIndex * sizeof(GLuint)));

    }
    materialBuffer.unbind();

    // renderNode(sceneGraphRoot, glm::mat4(1), VP);
    shaderProgram.unbind();
}

Camera &Scene::getActiveCamera() {
    return cameraController.getCameraByIndex(activeCameraIndex);
}

void Scene::buildMaterialBuffer() {
    std::vector<BufferMaterial > data;
    data.reserve(sceneBuffers.materials.size());
    for (auto &material: sceneBuffers.materials) {
        data.push_back(material.bufferMaterial);
    }

    materialBuffer = { static_cast<unsigned int>(data.size() * sizeof(BufferMaterial)), GL_STATIC_DRAW, data.data() };
}

Scene::~Scene() {
    shaderProgram.destroy();

    materialBuffer.destroy();
}

void Scene::reloadShaders() {
    shaderProgram.reloadShader();
}

void Scene::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Scene")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        startIndent();

        if (ImGui::Button("Reload shaders", buttonSize)) {
            reloadShaders();
        }

        endIndent();
    }

}
