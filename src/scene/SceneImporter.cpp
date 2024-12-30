#include <iostream>
#include "SceneImporter.h"
#include "tiny_gltf.h"
#include "scene/scene_loading/ModelLoader.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "scene/buffers/import_scene_error.h"
#include "scene/camera/Camera.h"
#include "BVHBuilder.h"
#include <algorithm>

/*
 * Reads an unsigned byte from 1 bytes at the specified address and casts it to
 * an unsigned int.
 */
unsigned int bytesFromUnsignedCharToUnsignedInt(unsigned char *address) {
    unsigned char out;
    memcpy(&out, address, sizeof(unsigned char));
    return (unsigned int) out;
}

/*
 * Reads an unsigned short from 2 bytes at the specified address and casts it to
 * an unsigned int.
 */
unsigned int bytesFromUnsignedShortToUnsignedInt(unsigned char *address) {
    unsigned short out;
    memcpy(&out, address, sizeof(unsigned short));
    return (unsigned int) out;
}

/*
 * Reads an unsigned int from 4 bytes at the specified address.
 */
unsigned int bytesToUnsignedInt(unsigned char *address) {
    unsigned int out;
    memcpy(&out, address, sizeof(unsigned int));
    return out;
}

SceneImporter::SceneImporter(std::string path)
        : scenePath(path) {

}

bool SceneImporter::import(CPUSceneBuffers &sceneBuffers, std::vector<Camera> &cameraList) {
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

    triangles = &sceneBuffers.triangles;
    objects = &sceneBuffers.objects;
    meshes = &sceneBuffers.meshes;
    transforms = &sceneBuffers.transforms;

    materials = &sceneBuffers.materials;

    emissives = &sceneBuffers.emissives;

    cameras = &cameraList;

    for (auto &gltfMat: model.materials) {
        auto &mat = materials->emplace_back();
        parseMaterial(gltfMat, mat);
    }
    // default material appendend to the end so every other material index still matches
    addDefaultMaterial();

    auto unityMat = glm::mat4(1);
    for (auto &node: model.scenes[model.defaultScene].nodes) {
        parseNode(model, node, unityMat);
    }

    return true;
}

void SceneImporter::parseMaterial(tinygltf::Material &source, Material &target) {
    target.name = source.name;
    BufferMaterial &bufferMat = target.bufferMaterial;
    auto &pbr = source.pbrMetallicRoughness;

    bufferMat.albedo = glm::make_vec4<double>(pbr.baseColorFactor.data());
    bufferMat.metallic = pbr.metallicFactor;
    bufferMat.roughness = pbr.roughnessFactor;
    bufferMat.roughness *= bufferMat.roughness;
    bufferMat.IOR = 1.5f;

    glm::vec3 emissionColor = glm::make_vec3<double>(source.emissiveFactor.data());
    bufferMat.transparency = 0;
    bufferMat.specular = glm::vec3(1);

    float factor = 1;
    for (auto &key_val: source.extensions) {
        if (key_val.first == "KHR_materials_emissive_strength") {
            if (key_val.second.IsObject()) {
                auto val = key_val.second.Get("emissiveStrength");
                if (val.IsNumber()) {
                    factor = val.GetNumberAsDouble();
                }
            }
        }

        if (key_val.first == "KHR_materials_transmission") {
            if (key_val.second.IsObject()) {
                auto val = key_val.second.Get("transmissionFactor");
                if (val.IsNumber()) {
                    bufferMat.transparency = val.GetNumberAsDouble();
                }
            }
        }

        if (key_val.first == "KHR_materials_ior") {
            if (key_val.second.IsObject()) {
                auto val = key_val.second.Get("ior");
                if (val.IsNumber()) {
                    bufferMat.IOR = val.GetNumberAsDouble();
                }
            }
        }

        if (key_val.first == "KHR_materials_specular") {
            if (key_val.second.IsObject()) {
                auto val = key_val.second.Get("specularColorFactor");
                if (val.IsArray()) {
                    bufferMat.specular = glm::vec3(val.Get(0).GetNumberAsDouble(),
                                                          val.Get(1).GetNumberAsDouble(),
                                                          val.Get(2).GetNumberAsDouble());
                }
            }
        }
    }

    bufferMat.emission = factor * glm::vec4(emissionColor, 1);
}

bool SceneImporter::parseNode(tinygltf::Model &model, int nodeId, const glm::mat4 &prevTransform) {
    tinygltf::Node &node = model.nodes[nodeId];

    glm::mat4 transform;
    buildTransform(node, transform);

    if (node.mesh != -1) {
        return parseObject(model, node.mesh, prevTransform * transform);
    } else if (node.camera != -1) {
        return parseCamera(model, node.camera, prevTransform * transform);
    } else {
        if (node.children.empty()) return true;

        for (auto &child: node.children) {
            parseNode(model, child, prevTransform * transform);
        }

        return true;
    }

}

bool SceneImporter::parseCamera(tinygltf::Model &model, int cameraId, const glm::mat4 &transform) {
    if (model.cameras.size() <= cameraId) return false;
    tinygltf::Camera &camera = model.cameras[cameraId];

    glm::vec3 camPos = transform * glm::vec4(0, 0, 0, 1);
    glm::vec3 viewDir = transform * glm::vec4(0, 0, -1, 0);

    Camera &cam = cameras->emplace_back(camera.name, camPos, viewDir);
   if (camera.type == "perspective") {
       Intrinsics intrinsics;
       intrinsics.fovY = camera.perspective.yfov;
       intrinsics.aspectRatio = camera.perspective.aspectRatio;
       intrinsics.zNear = camera.perspective.znear;
       intrinsics.zFar = camera.perspective.zfar;

        cam.setIntrinsics(intrinsics);
   }


    cam.buildCameraVisBuffers();
    return true;
}

void SceneImporter::buildTransform(tinygltf::Node &node, glm::mat4 &target) {
    glm::mat4 transform(1.0f);

    if (node.translation.size() == 3) {
        glm::vec3 translateVec(node.translation[0], node.translation[1], node.translation[2]);
        // translate = glm::translate(glm::mat4(1), translateVec);
        transform = glm::translate(glm::mat4(1), translateVec);
    }

    if (node.rotation.size() == 4) {
        transform *= makeRotationFromGltfQuat(node.rotation);
    }

    if (node.scale.size() == 3) {
        glm::vec3 scaleVec(node.scale[0], node.scale[1], node.scale[2]);
        transform = glm::scale(transform, scaleVec);
    }

    // TODO what if transform is given by matrix instead of vectors ? => extra check
    target = transform;
}

bool SceneImporter::parseObject(tinygltf::Model &model, int meshId, const glm::mat4 &transform) {
    tinygltf::Mesh gltfMesh = model.meshes[meshId];

    // TODO: Doesn't work like that, since I interpret gltfs mesh primitives as my objects
    //  Extend meshMap to hold pairs on ints, one for the gltf mesh id and one for its primitve id
    //  to map onto my own mesh interpretation, then check for mesh existence in those and create objects from that

    auto &accessors = model.accessors;
    auto &bufferViews = model.bufferViews;
    auto &buffers = model.buffers;

    for (int primitiveId = 0; primitiveId < gltfMesh.primitives.size(); primitiveId++) {
        auto &primitive = gltfMesh.primitives[primitiveId];

        auto& object = objects->emplace_back();
        auto it = meshesMap.find({meshId, primitiveId});

        object.transformID = transforms->size();

        glm::mat4 objectTransform = transform;
        transforms->push_back(objectTransform);

        // Check if this object's mesh already exists
        if (it != meshesMap.end()) {
            object.meshID = it->second;
            object.emissive = (*meshes)[it->second].bufferMesh.emissive;

            // If this object is emissive still register it as an emissive object
            // TODO build a test scene that has multiple lightsources with exactly the same mesh
            if (object.emissive > 0) {
                emissives->push_back(objects->size() - 1);
            }

            continue;
        }

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


        /* dont technically need those, accessors have min/max fields
            First attempt at using those didn't quite work.
            Keeping this for now because scene loading is offline and finding min/max should barely be slower
         */
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
        int indexStride = 1;
        int componentType = accessors[primitive.indices].componentType;

        // choose parsing function based on the numeric type that the index is stored as in the glTF file
        if (componentType == COMPONENT_TYPE_UINT) {
            parseIndex = bytesToUnsignedInt;
            indexStride = 4;
        } else if (componentType == COMPONENT_TYPE_USHORT) {
            parseIndex = bytesFromUnsignedShortToUnsignedInt;
            indexStride = 2;
        }

        int indexBufferIndex = bufferViews[primitive.indices].buffer;
        int indexOffset = bufferViews[primitive.indices].byteOffset;
        auto *indexBuffer = &buffers[indexBufferIndex].data;

        Mesh mesh;

        mesh.indexOffset = indices->size();
        mesh.bufferMesh.firstTriangleIndex = triangles->size();

        int materialIndex = primitive.material;

        // Some objects might have no material index -> will be -1, set to default material
        if (primitive.material < 0 | primitive.material >= materials->size()) {
            materialIndex = materials->size() - 1;
        }


        for (int i = 0; i < accessors[primitive.indices].count; i += 3) {
            unsigned int localIndex0 = parseIndex(&(*indexBuffer)[indexOffset + i * indexStride]);
            unsigned int localIndex1 = parseIndex(&(*indexBuffer)[indexOffset + (i + 1) * indexStride]);
            unsigned int localIndex2 = parseIndex(&(*indexBuffer)[indexOffset + (i + 2) * indexStride]);

            unsigned int index0 = localIndexOffset + localIndex0;
            unsigned int index1 = localIndexOffset + localIndex1;
            unsigned int index2 = localIndexOffset + localIndex2;

            indices->push_back(index0);
            indices->push_back(index1);
            indices->push_back(index2);

            auto &triangle = triangles->emplace_back();
            triangle.V0 = (*vertices)[index0].toBufferVertex();
            triangle.V1 = (*vertices)[index1].toBufferVertex();
            triangle.V2 = (*vertices)[index2].toBufferVertex();

            triangle.center = (triangle.V0.position + triangle.V1.position + triangle.V2.position) / glm::vec3(3);

            lower = glm::min(lower, triangle.V0.position);
            lower = glm::min(lower, triangle.V1.position);
            lower = glm::min(lower, triangle.V2.position);

            upper = glm::max(upper, triangle.V0.position);
            upper = glm::max(upper, triangle.V1.position);
            upper = glm::max(upper, triangle.V2.position);

            triangle.materialIndex = materialIndex;
        }


        constexpr float eps = 1e-2;
        lower -= glm::vec3(eps);
        upper += glm::vec3(eps);

        mesh.bufferMesh.triangleCount = triangles->size() - mesh.bufferMesh.firstTriangleIndex;
        mesh.indexCount = indices->size() - mesh.indexOffset;

        mesh.bufferMesh.aabb.lower = glm::vec4(lower, 1);
        mesh.bufferMesh.aabb.upper = glm::vec4(upper, 1);


        if (primitive.material >= 0 && primitive.material < materials->size() - 1) {
            auto &mat = (*materials)[primitive.material];
            glm::vec4 &emission = mat.bufferMaterial.emission;

            if (emission.x > eps || emission.y > eps || emission.z > eps) {
                mesh.bufferMesh.emissive = 1;
                object.emissive = 1;
                emissives->push_back(objects->size() - 1);
            }
        }

        // remember that we found and parsed this mesh already, if different objects uses same mesh
        meshesMap.insert({ {meshId, primitiveId}, meshes->size() });
        object.meshID = meshes->size();
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

void SceneImporter::addDefaultMaterial() {
    auto &mat = materials->emplace_back();

    mat.name = "default";
    auto &bufferMat = mat.bufferMaterial;
    bufferMat.albedo = glm::vec4(0.8, 0.8, 0.8, 1);
    bufferMat.roughness = 0.8f;
    bufferMat.metallic = 0.0f;
    bufferMat.IOR = 1.5;
    bufferMat.transparency = 0.0f;
    bufferMat.emission = glm::vec4(0);
}

glm::mat4 SceneImporter::makeRotationFromGltfQuat(const std::vector<double> &rotation) {
    float data[4];
    for (int i = 0; i < 4; i++) {
        // shift by one because tinygltf gives quats as xyzw but internally glm uses wxyz
        data[(i + 1) % 4] = static_cast<float>(rotation[i]);
    }
    return glm::mat4_cast(glm::make_quat(data));

}
