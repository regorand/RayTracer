#pragma once

#include <vector>
#include "renderers/Types.h"
#include "scene/Transformation.h"
#include "glWrappers/rasterization/IndexBuffer.h"
#include "glWrappers/rasterization/VertexArray.h"
#include "glWrappers/rasterization/ArrayBuffer.h"

/**
 * Represents a single, usually continous object made of triangles
 */
class MeshObject {
private:
    std::vector<Triangle> mTriangles;

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
