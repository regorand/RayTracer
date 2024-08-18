#include <iostream>
#include "ModelLoader.h"
#include "tiny_gltf.h"

bool loadModel(std::string path, tinygltf::Model &target) {
    tinygltf::TinyGLTF loader;
    std::string        errors;
    std::string        warnings;
    bool               success;


    success = loader.LoadASCIIFromFile(&target, &errors, &warnings, path);

    return success;
}
