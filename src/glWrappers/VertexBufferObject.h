#pragma once

#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/rasterization/ArrayBuffer.h"
#include "glWrappers/rasterization/IndexBuffer.h"
#include "tiny_gltf.h"

class VertexBufferObject {
private:
    VertexArray mVa;
    ArrayBuffer mAb;
    IndexBuffer mIb;


public:
    VertexBufferObject(tinygltf::BufferView bufferView, tinygltf::Buffer buffer);

};