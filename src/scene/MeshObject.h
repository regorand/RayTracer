#pragma once

#include <vector>
#include "renderers/Types.h"

/**
 * Represents a single, usually continous object made of triangles
 */
class MeshObject {
private:
    std::vector<Triangle> mTriangles;

    // TODO best option would be to have mapping from triangles into local material list, save local indices in \
        the triangles GPU buffer and store a constant offset in object buffer
    Material material;

    bool mIsEmissive = false;

public:
    // TODO
    // MeshObject();

    // TODO:
    /*
     * void load(std::string path);
     *
     */

    void addTriangle(Triangle triangle);

    std::vector<Triangle> &getTriangleBuffer();

    void setEmissive(bool emissive) { mIsEmissive = emissive; };
    bool isEmissive() const { return mIsEmissive ;};

};
