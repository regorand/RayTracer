#include <iostream>
#include "SceneGraphTraversal.h"

SceneGraphTraversal::SceneGraphTraversal(const tinygltf::Model &model, void *userData)
    : model(model), mUserData(userData)
{}

void SceneGraphTraversal::traverse() {

    for (int i = 0; i < model.scenes.size(); i++) {
        if (i < 0 || i >= model.scenes.size()) {
            errorMessage("no Scene found with index: " + std::to_string(i));
            continue;
        }

        const tinygltf::Scene &scene = model.scenes[i];
        traverseScene(scene, i);
    }
}

void SceneGraphTraversal::setUserData(void *userData) {
    mUserData = userData;
}

void SceneGraphTraversal::setSceneCallback(void (*callback)(const TraversalIndices &, const tinygltf::Scene &, void *)) {
    sceneCallBack = callback;
}

void SceneGraphTraversal::setInnerNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Node &, void *)) {
    innerCallBack = callback;
}

void SceneGraphTraversal::setMeshNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Mesh &, void *)) {
    meshNodeCallback = callback;
}

void SceneGraphTraversal::setMeshPrimitiveNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Primitive &, void *)) {
    primitiveNodeCallback = callback;
}

void SceneGraphTraversal::setCameraNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Camera &, void *)) {
    cameraNodeCallBack  = callback;
}

void SceneGraphTraversal::traverseScene(const tinygltf::Scene &scene, int index) {
    traversalIndices.currentScene = index;
    if (sceneCallBack != nullptr) {
        (sceneCallBack)(traversalIndices, scene, mUserData);
    }


    for (int nodeIndex: scene.nodes) {
        if (nodeIndex < 0 || nodeIndex >= model.nodes.size()) {
            errorMessage("No node found with in current scene with index " + std::to_string(nodeIndex));
            continue;
        }
        const tinygltf::Node &node = model.nodes[nodeIndex];

        traverseNode(node, nodeIndex);
    }

    traversalIndices.currentScene = -1;
}

void SceneGraphTraversal::traverseNode(const tinygltf::Node &node, int index) {
    traversalIndices.currentNode = index;

    if (node.mesh != -1) {
        if (innerCallBack != nullptr) {
            (innerCallBack)(traversalIndices, node, mUserData);
        }
        parseMesh(node.mesh);
    } else if (node.camera != -1) {
        parseCamera(node.camera);
    } else {
        if (node.children.empty()) return;

        for (int childIndex: node.children) {
            if (childIndex < 0 || childIndex >= model.nodes.size()) {
                errorMessage("No node found with in current scene with index " + std::to_string(childIndex));
                return;
            }

            traverseNode(model.nodes[childIndex], childIndex);
        }
    }

    traversalIndices.currentNode = -1;
}

void SceneGraphTraversal::errorMessage(const std::string errorMsg) {
    std::cerr << errorMsg << std::endl;
}

void SceneGraphTraversal::parseMesh(int index) {
    if (index < 0 || index >= model.meshes.size()) {
        errorMessage("No Mesh found with index: " + std::to_string(index));
        return;
    }

    traversalIndices.currentMesh = index;
    const tinygltf::Mesh &mesh = model.meshes[index];

    if (meshNodeCallback != nullptr) {
        (meshNodeCallback)(traversalIndices, mesh, mUserData);
    }

    for (int i = 0; i < mesh.primitives.size(); i++) {
        const tinygltf::Primitive &primitive = mesh.primitives[i];
        parsePrimitive(primitive, i);
    }
    traversalIndices.currentMesh = -1;
}

void SceneGraphTraversal::parsePrimitive(const tinygltf::Primitive &primitive, int index) {
    if (primitiveNodeCallback != nullptr) {
        traversalIndices.currentPrimitive = index;

        (primitiveNodeCallback)(traversalIndices, primitive, mUserData);

        traversalIndices.currentPrimitive = -1;
    }
}

void SceneGraphTraversal::parseCamera(int index) {
    if (index < 0 || index >= model.cameras.size()) {
        errorMessage("No Camera found with index: " + std::to_string(index));
        return;
    }

    if (cameraNodeCallBack != nullptr) {
        traversalIndices.currentCamera = index;

        const tinygltf::Camera &camera = model.cameras[index];
        (cameraNodeCallBack)(traversalIndices, camera, mUserData);

        traversalIndices.currentCamera = -1;
    }
}
