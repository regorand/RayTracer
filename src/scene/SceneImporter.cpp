#include <iostream>
#include "SceneImporter.h"
#include "tiny_gltf.h"
#include "scene/scene_loading/ModelLoader.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "scene/buffers/import_scene_error.h"

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

SceneImporter::SceneImporter(std::string path)
        : scenePath(path) {

}

bool SceneImporter::import(SceneBuffers &sceneBuffers) {
    tinygltf::Model model;
    bool result = loadModel(scenePath, model);
    if (!result) {
        std::cerr << "tinygltf could not load mode file " << scenePath << std::endl;
        return false;
    }

    if (model.scenes.size() > model.defaultScene + 1) {
        std::cerr << "gltf files require a default scene to be loaded" << std::endl;
        return false;
    }

    vertices = &sceneBuffers.vertices;
    indices = &sceneBuffers.indices;

    meshes = &sceneBuffers.meshes;
    transforms = &sceneBuffers.transforms;

    materials = &sceneBuffers.materials;

    emissives = &sceneBuffers.emissives;

    for (auto &gltfMat: model.materials) {
        auto &mat = materials->emplace_back();
        parseMaterial(gltfMat, mat);
    }

    for (auto &node: model.scenes[model.defaultScene].nodes) {
        parseNode(model, node, glm::mat4(1));
    }

    return true;
}

void SceneImporter::parseMaterial(tinygltf::Material &source, Material &target) {
    target.name = source.name;
    BufferMaterial &bufferMat = target.bufferMaterial;
    auto &pbr = source.pbrMetallicRoughness;
    bufferMat.albedo = glm::make_vec4<double>(pbr.baseColorFactor.data());

    glm::vec3 emissionColor = glm::make_vec3<double>(source.emissiveFactor.data());
    bufferMat.emission = glm::vec4(emissionColor, 1);

    bufferMat.metallic = pbr.metallicFactor;
    bufferMat.roughness = pbr.roughnessFactor;
    bufferMat.transparency = 0;
    bufferMat.IOR = 1;
    // TODO target.transparency: transparency from gltf extension ?
}

bool SceneImporter::parseNode(tinygltf::Model &model, int nodeId, glm::mat4 prevTransform) {
    tinygltf::Node &node = model.nodes[nodeId];

    glm::mat4 transform;
    buildTransform(node, transform);

    if (node.mesh != -1) {
        return parseMesh(model, node.mesh, transform * prevTransform);
    } else if (node.camera != -1) {
        // TODO can handle camera here
        return true;
    } else {
        if (node.children.empty()) return true;

        for (auto &child: node.children) {
            parseNode(model, child, transform * prevTransform);
        }

        return true;
    }

}

void SceneImporter::buildTransform(tinygltf::Node &node, glm::mat4 &target) {
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

bool SceneImporter::parseMesh(tinygltf::Model &model, int meshId, glm::mat4 transform) {
    tinygltf::Mesh gltfMesh = model.meshes[meshId];

    auto &accessors = model.accessors;
    auto &bufferViews = model.bufferViews;
    auto &buffers = model.buffers;

    for (auto &primitive: gltfMesh.primitives) {
        tinygltf::Accessor position = getAccessor(accessors, primitive, POSITION_ATTRIBUTE_NAME);
        tinygltf::Accessor normal = getAccessor(accessors, primitive, NORMAL_ATTRIBUTE_NAME);
        tinygltf::Accessor texCoord0 = getAccessor(accessors, primitive, TEX_0_ATTRIBUTE_NAME);

        if (!(position.count == normal.count && position.count == texCoord0.count)) {
            std::cerr << "vertex attribute counts for primitive do not match" << std::endl;
            return false;
        }

        int positionBufferIndex = bufferViews[position.bufferView].buffer;
        int positionOffset = bufferViews[position.bufferView].byteOffset;
        auto *positionBuffer = &buffers[positionBufferIndex].data;

        int normalBufferIndex = bufferViews[normal.bufferView].buffer;
        int normalOffset = bufferViews[normal.bufferView].byteOffset;
        auto *normalBuffer = &buffers[normalBufferIndex].data;

        int texCoordBufferIndex = bufferViews[texCoord0.bufferView].buffer;
        int texCoordOffset = bufferViews[texCoord0.bufferView].byteOffset;
        auto *texCoordBuffer = &buffers[texCoordBufferIndex].data;

        // Each Primitive has local indices, i.e. starting at 0
        // Since I build one global vertex buffer, need to offset the indices by the vertices already in global buffer
        int localIndexOffset = vertices->size();

        Mesh mesh;
        mesh.bufferMesh.transformIndex = transforms->size();

        transforms->push_back(transform);

        // TODO dont need these, there are min/max fields in gltf file
        glm::vec3 lower(INFINITY);
        glm::vec3 upper(-INFINITY);

        for (int i = 0; i < position.count; i++) {
            auto positionAddress = &(*positionBuffer)[positionOffset + i * FLOAT_SIZE * 3];
            auto normalAddress = &(*normalBuffer)[normalOffset + i * FLOAT_SIZE * 3];
            auto texCoordAddress = &(*texCoordBuffer)[texCoordOffset + i * FLOAT_SIZE * 2];

            vertices->push_back({glm::make_vec3((float *) positionAddress),
                                glm::make_vec3((float *) normalAddress),
                                glm::make_vec2((float *) texCoordAddress),
                                primitive.material});
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

        mesh.indexOffset = indices->size();
        mesh.bufferMesh.firstTriangleIndex = triangles.size();

        for(int i = 0; i < accessors[primitive.indices].count; i+=3) {
            unsigned int localIndex0 = parseIndex(&(*indexBuffer)[indexOffset + i * indexStride]);
            unsigned int localIndex1 = parseIndex(&(*indexBuffer)[indexOffset + (i + 1) * indexStride]);
            unsigned int localIndex2 = parseIndex(&(*indexBuffer)[indexOffset + (i + 2) * indexStride]);

            unsigned int index0 = localIndexOffset + localIndex0;
            unsigned int index1 = localIndexOffset + localIndex1;
            unsigned int index2 = localIndexOffset + localIndex2;

            indices->push_back(index0);
            indices->push_back(index1);
            indices->push_back(index2);

            auto &triangle = triangles.emplace_back();
            triangle.V0 = (*vertices)[index0].vertex;
            triangle.V1 = (*vertices)[index1].vertex;
            triangle.V2 = (*vertices)[index2].vertex;

            lower = glm::min(lower, triangle.V0.position);
            lower = glm::min(lower, triangle.V1.position);
            lower = glm::min(lower, triangle.V2.position);

            upper = glm::max(upper, triangle.V0.position);
            upper = glm::max(upper, triangle.V1.position);
            upper = glm::max(upper, triangle.V2.position);

            triangle.materialIndex = primitive.material;
        }


        mesh.bufferMesh.triangleCount = triangles.size() - mesh.bufferMesh.triangleCount;
        mesh.indexCount = indices->size() - mesh.indexOffset;

        mesh.bufferMesh.aabb.lower = lower;
        mesh.bufferMesh.aabb.upper = upper;

        auto &mat = (*materials)[primitive.material];
        if (glm::length(mat.bufferMaterial.emission) > 1e-2) {
            mesh.bufferMesh.emissive = 1;
            emissives->push_back(meshes->size());
        }

        meshes->push_back(mesh);
    }

    return true;
}

tinygltf::Accessor
SceneImporter::getAccessor(std::vector<tinygltf::Accessor> &accessors, tinygltf::Primitive &primitive,
                           const std::string &attribute) {
    if (primitive.attributes.count(attribute) == 0) {
        throw import_scene_error("primitive does not have attribute: " + attribute);
    }
    return accessors[primitive.attributes[attribute]];

}
