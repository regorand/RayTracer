
#include <stack>
#include "Scene.h"
#include "scene/scene_loading/ModelLoader.h"
#include "scene/buffers/SceneBuilder.h"
#include "scene/buffers/import_scene_error.h"
#include "Vertex.h"
#include "glm/gtc/matrix_inverse.hpp"

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

    return true;
}

void Scene::buildRasterBuffers() {
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

    /*
     * TODO
     * 1. build global interlaced Vertex Buffer
     *  -> If a Vertex already exists, but needs different colors, copy it. Not sure if this will happen
     * 2. build global Index Buffer
     * 3. for each object keep track index into index Buffer and index count
     *  -> Store in Mesh object (or MeshIndex ?)
     * 4. For Rendering:
     *  -> Iterate SceneGraph
     *  ->
     *
     *
     */

}

void Scene::buildPathtraceBuffers() {}

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

    Shader shader("res/shaders/raster/test_new.vert", "res/shaders/raster/test_new.frag");

    mRasterBuffers.mVa.bind();
    mRasterBuffers.mIb.bind();

    shader.bind();
    renderNode(sceneGraphRoot, shader, glm::mat4(1), VP);
}

void Scene::renderNode(GraphNode &node, Shader &shader, glm::mat4 transform, glm::mat4 &VP) {
    if (node.type == NodeType::INNER) {
        for (auto &child: node.children) {
            renderNode(child, shader, node.transform * transform, VP);
        }
    }

    if (node.type != NodeType::MESH) return;

    Mesh &mesh = mMeshes[node.index];

    glm::mat4 model = node.transform * transform;
    glm::mat4 MVP = VP * model; // * transform;
    glm::mat4 normalTransform = glm::inverseTranspose(transform);

    shader.setUniformMat4f("transform", model);
    shader.setUniformMat4f("MVP", MVP);

    shader.setUniformMat4f("normalTransform", normalTransform);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    int indexBufferIndex = mesh.getIndexOffset();
    int indexCount = mesh.getIndexCount();


    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void *) (indexBufferIndex * sizeof(GLuint)));
}
