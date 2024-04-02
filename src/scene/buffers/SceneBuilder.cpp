
#include <stack>
#include "SceneBuilder.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "import_scene_error.h"
#include "scene/SceneGraph.h"
#include "scene/Material.h"
#include <glm/gtx/quaternion.hpp>

/*
 * Reads an unsigned byte from 1 bytes at the specified address and casts it to
 * an unsigned int.
 */
unsigned int bytesFromUnsignedCharToUnsignedInt(unsigned char* address) {
    unsigned char out;
    memcpy(&out, address, sizeof(unsigned char));
    return (unsigned int)out;
}

/*
 * Reads an unsigned short from 2 bytes at the specified address and casts it to
 * an unsigned int.
 */
unsigned int bytesFromUnsignedShortToUnsignedInt(unsigned char* address) {
    unsigned short out;
    memcpy(&out, address, sizeof(unsigned short));
    return (unsigned int)out;
}

/*
 * Reads an unsigned int from 4 bytes at the specified address.
 */
unsigned int bytesToUnsignedInt(unsigned char* address) {
    unsigned int out;
    memcpy(&out, address, sizeof(unsigned int));
    return out;
}

bool SceneBuilder::build(tinygltf::Model &model,
                         std::vector<Mesh> &targetMeshes,
                         std::vector<Camera> &targetCameras,
                         std::vector<Material> &materials,
                         GraphNode &root) {

    /*
     * TODO:
     * - Create Scenegraph
     * - Store Transformations in scene graph
     * - build functionality to build buffers
     */

    if (model.scenes.size() > model.defaultScene + 1) {
        throw std::runtime_error("Programm currently needs valid default scene");
    }

    std::vector<tinygltf::Camera> &cameras = model.cameras;


    IterateValues iterateValues(targetMeshes, targetCameras);

    for (auto &gltfMat: model.materials) {
        auto &mat = materials.emplace_back();
        parseMaterial(gltfMat, mat);
    }

    for (auto &node: model.scenes[model.defaultScene].nodes) {
        parseNode(model, node, iterateValues, root);
    }

    return true;
}

bool SceneBuilder::parseNode(tinygltf::Model &model, int nodeId, IterateValues &iterateValues, GraphNode &parentNode) {
    tinygltf::Node &node = model.nodes[nodeId];

    GraphNode root;

    glm::mat4 transform;
    buildTransform(node, transform);

    auto &newNode = parentNode.children.emplace_back();
    newNode.transform = transform;

    if (node.mesh != -1) {
        return parseMesh(model, node.mesh, iterateValues, newNode);
    } else if (node.camera != -1) {
        // TODO handle camera
        parseCamera(model, node.camera, iterateValues, parentNode.transform, transform);
        return true;
    } else {
        if (node.children.empty()) return true;

        for (auto& child: node.children) {
            parseNode(model, child, iterateValues, newNode);
        }

        return true;
    }
}

void SceneBuilder::buildTransform(tinygltf::Node &node, glm::mat4 &target) {
    glm::mat4 translate(1);
    glm::mat4 scale(1);
    glm::mat4 rotate(1);


    if (node.translation.size() == 3) {
        glm::vec3 translateVec(node.translation[0], node.translation[1], node.translation[2]);
        translate = glm::translate(glm::mat4(1), translateVec);
    }

    if (node.rotation.size() == 4) {
        rotate = glm::toMat4(glm::make_quat(node.rotation.data()));
    }

    if (node.scale.size() == 3) {
        glm::vec3 scaleVec(node.scale[0], node.scale[1], node.scale[2]);
        scale = glm::scale(glm::mat4(1), scaleVec);
    }

    // TODO what if transform is given by matrix instead of vectors ? => extra check
    target = translate * rotate * scale;
}

bool SceneBuilder::parseMesh(tinygltf::Model &model, int meshId, IterateValues &iterateValues, GraphNode &meshNode) {
    tinygltf::Mesh gltfMesh = model.meshes[meshId];

    auto &accessors = model.accessors;
    auto &bufferViews = model.bufferViews;
    auto &buffers = model.buffers;


    meshNode.index = iterateValues.objects.size();
    Mesh &mesh = iterateValues.objects.emplace_back( gltfMesh.name );
    meshNode.type = NodeType::MESH;

    std::vector<VertexOld> vertices;
    std::vector<unsigned int> indices;

    for (auto &primitive: gltfMesh.primitives) {

        tinygltf::Accessor position = getAccessor(accessors, primitive, POSITION_ATTRIBUTE_NAME);
        tinygltf::Accessor normal = getAccessor(accessors, primitive, NORMAL_ATTRIBUTE_NAME);
        tinygltf::Accessor texCoord0 = getAccessor(accessors, primitive, TEX_0_ATTRIBUTE_NAME);

        if (!(position.count == normal.count && position.count == texCoord0.count)) {
            throw import_scene_error("vertex attribute counts for primitive do not match");
        }

        int   positionBufferIndex = bufferViews[position.bufferView].buffer;
        int   positionOffset      = bufferViews[position.bufferView].byteOffset;
        auto* positionBuffer      = &buffers[positionBufferIndex].data;

        int   normalBufferIndex = bufferViews[normal.bufferView].buffer;
        int   normalOffset      = bufferViews[normal.bufferView].byteOffset;
        auto* normalBuffer      = &buffers[normalBufferIndex].data;

        int   texCoordBufferIndex = bufferViews[texCoord0.bufferView].buffer;
        int   texCoordOffset      = bufferViews[texCoord0.bufferView].byteOffset;
        auto* texCoordBuffer      = &buffers[texCoordBufferIndex].data;

        for(int i = 0; i < position.count; i++) {
            // last number from the multiplication is the number of components in the vector
            auto positionAddress = &(*positionBuffer)[positionOffset + i * FLOAT_SIZE * 3];
            auto normalAddress = &(*normalBuffer)[normalOffset + i * FLOAT_SIZE * 3];
            auto texCoordAddress = &(*texCoordBuffer)[texCoordOffset + i * FLOAT_SIZE * 2];

            auto vec = glm::make_vec3((float *) positionAddress);
            vertices.push_back({
                // glm::vec3(vec.y, vec.z, vec.x),
                vec,
                glm::make_vec3((float *) normalAddress),
                glm::make_vec2((float *) texCoordAddress)
            });
        }

        // default parsing function (if indices are stored as byte)
        auto parseIndex = bytesFromUnsignedCharToUnsignedInt;
        // size of one index in bytes
        int indexStride   = 1;
        int componentType = accessors[primitive.indices].componentType;

        // choose parsing function based on the numeric type that the index is stored as in the glTF file
        if(componentType == COMPONENT_TYPE_UINT) {
            parseIndex  = bytesToUnsignedInt;
            indexStride = 4;
        } else if(componentType == COMPONENT_TYPE_USHORT) {
            parseIndex  = bytesFromUnsignedShortToUnsignedInt;
            indexStride = 2;
        }

        int   indexBufferIndex = bufferViews[primitive.indices].buffer;
        int   indexOffset      = bufferViews[primitive.indices].byteOffset;
        auto* indexBuffer      = &buffers[indexBufferIndex].data;

        // convert indices into internal format
        for(int i = 0; i < accessors[primitive.indices].count; i++) {
            auto indexAddress  = &(*indexBuffer)[indexOffset + i * indexStride];
            unsigned int index = parseIndex(indexAddress);

            indices.push_back(index);
        }

        mesh.addPrimitive({vertices, indices, primitive.material});
    }

    return true;
}

tinygltf::Accessor SceneBuilder::getAccessor(std::vector<tinygltf::Accessor> &accessors,
                                             tinygltf::Primitive &primitive,
                                             const std::string &attribute) {
    if (primitive.attributes.count(attribute) == 0) {
        throw import_scene_error("primitive does not have attribute: " + attribute);
    }
    return accessors[primitive.attributes[attribute]];
}

void SceneBuilder::parseMaterial(tinygltf::Material &source, Material &target) {
    target.name = source.name;
    BufferMaterial &bufferMat = target.bufferMaterial;
    auto &pbr = source.pbrMetallicRoughness;
    bufferMat.albedo = glm::make_vec4<double>(pbr.baseColorFactor.data());

    glm::vec3 emissionColor = glm::make_vec3<double>(source.emissiveFactor.data());
    bufferMat.emission = glm::vec4(emissionColor, 1);

    bufferMat.metallic = pbr.metallicFactor;
    bufferMat.roughness = pbr.roughnessFactor;
    // TODO target.transparency: transparency from gltf extension ?

}

void
SceneBuilder::parseCamera(tinygltf::Model &model, int cameraId, IterateValues &iterateValues, glm::mat4 &transform, glm::mat4 &lookTransform) {
    tinygltf::Camera &gltfCam = model.cameras[cameraId];

    if (gltfCam.type != "perspective") {
        std::cout << "LOG: only perspective cameras are supported at the moment" << std::endl;
    }

    Intrinsics intrinsics;
    intrinsics.fovY = gltfCam.perspective.yfov;
    intrinsics.aspectRatio = gltfCam.perspective.aspectRatio;

    intrinsics.zNear = gltfCam.perspective.znear;
    intrinsics.zFar = gltfCam.perspective.zfar;

    glm::vec3 pos = transform * glm::vec4(0, 0, 0, 1);
    glm::vec3 lookDir = lookTransform * glm::vec4(0, 0, -1, 0);

    Camera camera(pos, lookDir);
    camera.setName(gltfCam.name);
    camera.setIntrinsics(intrinsics);

    iterateValues.targetCameras.emplace_back(camera);
}
