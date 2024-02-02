#pragma once

#include <vector>
#include "renderers/Types.h"
#include "scene/Transformation.h"
#include "glWrappers/IndexBuffer.h"
#include "glWrappers/VertexArray.h"
#include "glWrappers/ArrayBuffer.h"

/**
 * Represents a single, usually continous object made of triangles
 */
class MeshObject {
private:
    std::vector<Triangle> mTriangles;
    std::vector<Material> mMaterials;

    // TODO best option would be to have mapping from triangles into local material list, save local indices in \
        the triangles GPU buffer and store a constant offset in object buffer
    bool mIsEmissive = false;

    Transformation mTransform;

    VertexArray mVa;
    ArrayBuffer mAb;
    IndexBuffer mIb;

public:
    // TODO
    // MeshObject();

    // TODO:
    /*
     * void load(std::string path);
     *
     */

    void setTransform(Transformation transformation);
    Transformation getTransform();
    Transformation &getTransformRef();

    void addTriangle(Triangle triangle);

    std::vector<Triangle> &getTriangles();

    AABB createAABB();

    std::vector<Triangle> &getTriangleBuffer();

    void setEmissive(bool emissive) { mIsEmissive = emissive; };
    bool isEmissive();

    bool canRasterize();

    VertexArray &getVertexArray();
    IndexBuffer &getIndexBuffer();

    void createRasterBuffers();

    bool isTransmissive();

};
