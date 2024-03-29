#pragma once

#include "tiny_gltf.h"
#include "SceneBuffersOld.h"
#include "scene/camera/Camera.h"
#include "scene/Mesh.h"
#include "scene/SceneGraph.h"
#include "scene/Material.h"
#include <stack>

constexpr int FLOAT_SIZE  = 4;

constexpr int COMPONENT_TYPE_UBYTE  = 5121;
constexpr int COMPONENT_TYPE_USHORT = 5123;
constexpr int COMPONENT_TYPE_UINT   = 5125;

const std::string POSITION_ATTRIBUTE_NAME = "POSITION";
const std::string NORMAL_ATTRIBUTE_NAME = "NORMAL";
const std::string TEX_0_ATTRIBUTE_NAME = "TEXCOORD_0";

struct IterateValues {
    std::vector<Mesh> &objects;
    std::vector<Camera> &targetCameras;

    IterateValues(std::vector<Mesh> &objects,
                  std::vector<Camera> &targetCameras)
        :objects(objects), targetCameras(targetCameras)
    {}
};

class SceneBuilder {
public:
    bool build(tinygltf::Model &model,
               std::vector<Mesh> &targetMeshes,
               std::vector<Camera> &targetCameras,
               std::vector<Material> &materials,
               GraphNode &root);

private:
    static void buildTransform(tinygltf::Node &node, glm::mat4 &target);

    bool parseNode(tinygltf::Model &model, int nodeId, IterateValues &iterateValues, GraphNode &parentNode);

    bool parseMesh(tinygltf::Model &model, int meshId, IterateValues &iterateValues, GraphNode &meshNode);

    tinygltf::Accessor getAccessor(std::vector<tinygltf::Accessor> &accessors,
                                   tinygltf::Primitive &primitive,
                                   const std::string &attribute);

    void parseMaterial(tinygltf::Material &source, Material &target);

    void parseCamera(tinygltf::Model &model, int cameraId, IterateValues &iterateValues, glm::mat4 &transform, glm::mat4 &lookTransform);
};
