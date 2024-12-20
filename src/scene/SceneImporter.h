#pragma once

#include <string>
#include <vector>
#include "SceneBuffers.h"
#include "Material.h"
#include "tiny_gltf.h"
#include "scene/camera/Camera.h"

constexpr int FLOAT_SIZE  = 4;

constexpr int COMPONENT_TYPE_UBYTE  = 5121;
constexpr int COMPONENT_TYPE_USHORT = 5123;
constexpr int COMPONENT_TYPE_UINT   = 5125;

const std::string POSITION_ATTRIBUTE_NAME = "POSITION";
const std::string NORMAL_ATTRIBUTE_NAME = "NORMAL";
const std::string TEX_0_ATTRIBUTE_NAME = "TEXCOORD_0";

typedef struct MeshIdentifier {
    int meshId {};
    int primitveId {};

    MeshIdentifier() = default;
    MeshIdentifier(int meshId, int primitiveId) : meshId(meshId), primitveId(primitiveId) {}

    bool operator== (MeshIdentifier const & other) const {
        return this->meshId == other.meshId && this->primitveId == other.primitveId;
    }

    bool operator< (MeshIdentifier const & other) const {
        if (this->meshId < other.meshId) return true;
        if (this->meshId > other.meshId) return false;

        if (this->primitveId < other.primitveId) return true;
        if (this->primitveId > other.primitveId) return false;

        return false;
    }
} MeshIdentifier;

class SceneImporter {
private:
    const std::string scenePath;

    std::vector<RasterVertex> *vertices;
    std::vector<unsigned int> *indices;

    std::map<MeshIdentifier, int> meshesMap;

    std::vector<Mesh> *meshes;
    std::vector<Object> *objects;
    std::vector<glm::mat4> *transforms;
    std::vector<unsigned int> *emissives;
    std::vector<Material> *materials;
    std::vector<Triangle> *triangles;

    std::vector<Camera> *cameras;

public:
    SceneImporter(std::string path);

    bool import(CPUSceneBuffers &sceneBuffers, std::vector<Camera> &cameraList);

private:
    bool parseNode(tinygltf::Model &model, int nodeId, const glm::mat4 &prevTransform);
    bool parseObject(tinygltf::Model &model, int meshId, const glm::mat4 &transform);
    bool parseCamera(tinygltf::Model &model, int cameraId, const glm::mat4 &transform);


    static void buildTransform(tinygltf::Node &node, glm::mat4 &target);
    static glm::mat4 makeRotationFromGltfQuat(const std::vector<double> &rotation);

    void parseMaterial(tinygltf::Material &source, Material &target);

    tinygltf::Accessor getAccessor(std::vector<tinygltf::Accessor> &accessors,
                                   tinygltf::Primitive &primitive,
                                   const std::string &attribute);

    void addDefaultMaterial();
};
