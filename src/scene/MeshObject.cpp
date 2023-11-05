
#include "MeshObject.h"

void MeshObject::addTriangle(Triangle triangle) {
    mTriangles.push_back(triangle);
}

std::vector<Triangle> &MeshObject::getTriangleBuffer() {
    return mTriangles;
}
