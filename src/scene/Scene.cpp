
#include "Scene.h"
#include "scene/scene_loading/ModelLoader.h"
#include "scene/buffers/SceneBuilder.h"
#include "scene/buffers/import_scene_error.h"
#include "Vertex.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "imgui.h"
#include "gui/ImguiUtils.h"
#include "glWrappers/shaders/ShaderUtils.hpp"

void Scene::addCamera(Camera camera) {
    cameraController.addCamera(camera);
}

bool Scene::loadFile(std::string path) {
    tinygltf::Model model;
    loadModel(path, model);

    std::vector<Camera> newCams;

    SceneBuilder builder;
    builder.build(model, mMeshes, newCams, mMaterials, sceneGraphRoot);

    if (!newCams.empty()) {
        cameraController.addCameras(newCams);
    }

    shaderProgram = makeRasterizeProgram("res/shaders/raster/test_new.vert", "res/shaders/raster/test_new.frag", "material_test_raster");

    return true;
}

void Scene::buildBuffers() {
    std::vector<Vertex> data;
    std::vector<unsigned int> indices;

    buildRasterBuffersNode(sceneGraphRoot, data, indices);


    mRasterBuffers.mVa.bind();

    GLsizei stride = sizeof(Vertex);

    mRasterBuffers.mAb = ArrayBuffer(data.data(), sizeof(Vertex) * data.size(), false);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0); // pos
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *) sizeof(glm::vec3)); // normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, stride, (void *) (2 * sizeof(glm::vec3))); // material
    glEnableVertexAttribArray(2);


    mRasterBuffers.mIb = IndexBuffer(indices.data(), indices.size());

    buildMaterialBuffer();
    // TODO for Path Tracing
    /* 1. Reuse ArrayBuffer
     * 2. Reuse IndexBuffer -> Until there is some kind of acceleration data structure, then store indices in there
     * 3. Reuse MaterialBuffer
     * 4. Needs some kind of ObjectBuffer
     *  - Holds Transforms
     *  - Holds baseIndex and indexOffset into indices buffer
     *  - should be indexed for emissive objects
     * 5. Needs some kind of EmissiveBuffer
     *  - First Version: only index into ObjectBuffer
     *  - Then: also hold information for light sampling (distance, area, etc)
     *
     * General Idea for Acceleration Structure
     * 1. Top level corresponds to objects (object instances), somewhat dynmic when transforms change
     * 2. Bottom level corresponds to the triangles within objects, relatively static
     *
     * Probably just build this with the RasterBuffers ?
     */


    /*
     * TODO some concrete tasks
     * - MeshPrimitives need to store indexOffset and indexCount
     *
     */
}

void Scene::buildRasterBuffersNode(GraphNode node, std::vector<Vertex> &data, std::vector<unsigned int> &indices) {
    if (node.type == NodeType::CAMERA) return;

    if (node.type == NodeType::INNER) {
        for (auto &child: node.children) buildRasterBuffersNode(child, data, indices);
        return;
    }

    if (node.index < 0 || node.index >= mMeshes.size()) {
        throw import_scene_error("Failed to build rasterize buffers because index of a mesh node does not match meshes vector");
    }
    Mesh &mesh = mMeshes[node.index];

    int vertexOffset = data.size();
    int indexOffset = indices.size();

    int indexCount = mesh.fillBuffers(data, indices, vertexOffset);

    mesh.setIndexOffset(indexOffset);
    mesh.setIndexCount(indexCount);
}

void Scene::render(Camera &camera) {
    glm::mat4 cameraMat = camera.getTransformation();
    glm::mat4 projectionMat = camera.getProjection();

    glm::mat4 VP = projectionMat * cameraMat;

    glEnable(GL_DEPTH_TEST);

    mRasterBuffers.mVa.bind();
    mRasterBuffers.mIb.bind();

    shaderProgram.bind();
    materialBuffer.bind();
    renderNode(sceneGraphRoot, glm::mat4(1), VP);
    shaderProgram.unbind();
}

void Scene::renderNode(GraphNode &node, glm::mat4 transform, glm::mat4 &VP) {
    if (node.type == NodeType::INNER) {
        for (auto &child: node.children) {
            renderNode(child, node.transform * transform, VP);
        }
    }

    if (node.type != NodeType::MESH) return;

    Mesh &mesh = mMeshes[node.index];

    glm::mat4 model = node.transform * transform;
    glm::mat4 MVP = VP * model; // * transform;
    glm::mat4 normalTransform = glm::inverseTranspose(transform);

    shaderProgram.setUniformMat4f("transform", model);
    shaderProgram.setUniformMat4f("MVP", MVP);

    shaderProgram.setUniformMat4f("normalTransform", normalTransform);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    int indexBufferIndex = mesh.getIndexOffset();
    int indexCount = mesh.getIndexCount();


    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void *) (indexBufferIndex * sizeof(GLuint)));
}

Camera &Scene::getActiveCamera() {
    return cameraController.getCameraByIndex(activeCameraIndex);
}

void Scene::buildMaterialBuffer() {
    /*
    std::vector<glm::vec4> data;
    for (auto &material: mMaterials) {
        data.push_back(material.albedo);
    }

    materialBuffer = {data.data(), data.size() * sizeof(glm::vec4), GL_STATIC_READ, 3};
*/
    std::vector<BufferMaterial > data;
    data.reserve(mMaterials.size());
    for (auto &material: mMaterials) {
        data.push_back(material.bufferMaterial);
    }

    materialBuffer = {data.data(), data.size() * sizeof(BufferMaterial), GL_STATIC_READ, 3};

    // materialBuffer = { GL_SHADER_STORAGE_BUFFER, GL_STATIC_READ, static_cast<unsigned int>(data.size() * sizeof(glm::vec4)), data.data() };
}

Scene::~Scene() {
    shaderProgram.destroy();
}

void Scene::reloadShaders() {
    shaderProgram.reloadShader();
}

void Scene::buildImguiTab(DataStore &settings, std::vector<Event> &events) {
    if (ImGui::CollapsingHeader("Scene")) {

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonSize = ImVec2(windowSize.x * 0.9, 25);

        startIndent();

        if (ImGui::Button("Reload compute shaders", buttonSize)) {
            reloadShaders();
        }

        endIndent();
    }

}
