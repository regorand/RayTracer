#pragma once

#include "tiny_gltf.h"

typedef struct traversal_index_s {
    int currentScene {-1};
    int currentNode {-1};
    int currentMesh {-1};
    int currentPrimitive {-1};
    int currentCamera {-1};
} TraversalIndices;


inline void defaultSceneCallback(const TraversalIndices &traversalIndices, const tinygltf::Scene &scene, void *userData) {
    std::cout << "Traversed Scene \"" << scene.name << "\" with index " << traversalIndices.currentScene << std::endl;
}

inline void defaultInnerCallback(const TraversalIndices &traversalIndices, const tinygltf::Node &node, void *userData) {
    std::cout << "Traversed Inner Node \"" << node.name << "\" with index " << traversalIndices.currentNode << std::endl;
}

inline void defaultMeshCallback(const TraversalIndices &traversalIndices, const tinygltf::Mesh &mesh, void *userData) {
    std::cout << "Traversed Mesh \"" << mesh.name << "\" with index " << traversalIndices.currentMesh << std::endl;
}

inline void defaultPrimitiveCallback(const TraversalIndices &traversalIndices, const tinygltf::Primitive &primitive, void *userData) {
    std::cout << "Traversed Primitive with index " << traversalIndices.currentPrimitive << std::endl;
}

inline void defaultCameraCallback(const TraversalIndices &traversalIndices, const tinygltf::Camera &camera, void *userData) {
    std::cout << "Traversed Camera \"" << camera.name << "\" with index " << traversalIndices.currentCamera << std::endl;
}

/*
 * This class enables traversing a tinygltf Model as a scenegraph
 * Callbacks can be set for various types of objects
 * Each callback will be called with:
 * 1. an index of the object within the corresponding tinygltf vector
 * 2. a const reference to the object
 * 3. a void pointer to some user defined data. By default this is nullptr
 *
 */


class SceneGraphTraversal {
    const tinygltf::Model &model;
    void* mUserData;

    void (*sceneCallBack)           (const TraversalIndices &, const tinygltf::Scene &,      void *) = nullptr;
    void (*innerCallBack)           (const TraversalIndices &, const tinygltf::Node &,       void *) = nullptr;
    void (*meshNodeCallback)        (const TraversalIndices &, const tinygltf::Mesh &,       void *) = nullptr;
    void (*primitiveNodeCallback)   (const TraversalIndices &, const tinygltf::Primitive &,  void *) = nullptr;
    void (*cameraNodeCallBack)      (const TraversalIndices &, const tinygltf::Camera &,     void *) = nullptr;

    TraversalIndices traversalIndices;
public:
    explicit SceneGraphTraversal(const tinygltf::Model &model, void *userData);

    void traverse();

    void setUserData(void *userData);

    void setSceneCallback(void (*callback)(const TraversalIndices &, const tinygltf::Scene &, void *));
    void setInnerNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Node &, void *));

    void setMeshNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Mesh &, void *));
    void setMeshPrimitiveNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Primitive &, void *));

    void setCameraNodeCallback(void (*callback)(const TraversalIndices &, const tinygltf::Camera &, void *));

private:
    void traverseScene(const tinygltf::Scene &scene, int index);

    void traverseNode(const tinygltf::Node &node, int index);

    void parseMesh(int index);
    void parsePrimitive(const tinygltf::Primitive &primitive, int index);

    void parseCamera(int index);

    void errorMessage(std::string errorMsg);
};
