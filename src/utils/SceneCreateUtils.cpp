#include "SceneCreateUtils.h"
#include "deviceShared/MaterialFlags.h"

void createCornellBox(std::vector<MeshObject> &outObjects) {
    MeshObject walls;

    std::vector<Vertex> wallVertices;

    glm::vec3 lower(-10);
    glm::vec3 higher(10);

    glm::vec3 offset(0);

    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            for (int z = 0; z < 2; z++) {
                Vertex v;
                v.pos = (glm::vec3(1) - glm::vec3(x, y, z)) * lower + glm::vec3(x, y, z) * higher + offset;
                wallVertices.push_back(v);
            }
        }
    }


    Material mirror;
    // mirror.boolsSpecTrans = glm::vec4(1, 0, 0, 0);
    unsigned int matProperties = SPECULAR_FLAG;
    mirror.color.w = reinterpret_cast<float &>(matProperties);

    // left wall
    walls.addTriangle(createTriangle(wallVertices, 1, 0, 2, mirror));
    walls.addTriangle(createTriangle(wallVertices, 1, 2, 3, mirror));

    // ground
    walls.addTriangle(createTriangle(wallVertices, 0, 1, 4, DIFFUSE_BLUE));
    walls.addTriangle(createTriangle(wallVertices, 1, 5, 4, DIFFUSE_BLUE));

    // right wall
    walls.addTriangle(createTriangle(wallVertices, 4, 5, 7, DIFFUSE_GREEN));
    walls.addTriangle(createTriangle(wallVertices, 4, 7, 6, DIFFUSE_GREEN));

    // back wall
    walls.addTriangle(createTriangle(wallVertices, 0, 4, 6, DIFFUSE_WHITE));
    walls.addTriangle(createTriangle(wallVertices, 0, 6, 2, DIFFUSE_WHITE));

    // ceiling
    walls.addTriangle(createTriangle(wallVertices, 2, 6, 7, DIFFUSE_WHITE));
    walls.addTriangle(createTriangle(wallVertices, 2, 7, 3, DIFFUSE_WHITE));

    MeshObject lightSource;

    float xDiff = higher.x - lower.x;
    float zDiff = higher.z - lower.z;

    float percentage = 0.5;
    float eps = 1e-5;

    glm::vec2 lightLower =
            glm::vec2(lower.x, lower.y) + glm::vec2((xDiff * 0.5) * percentage, (zDiff * 0.5) * percentage);
    glm::vec2 lightHigher =
            glm::vec2(higher.x, higher.y) - glm::vec2((xDiff * 0.5) * percentage, (zDiff * 0.5) * percentage);

    Vertex LL;
    LL.pos = glm::vec3(lightLower.x, higher.y - eps, lightLower.y);

    Vertex LU;
    LU.pos = glm::vec3(lightLower.x, higher.y - eps, lightHigher.y);

    Vertex UL;
    UL.pos = glm::vec3(lightHigher.x, higher.y - eps, lightLower.y);

    Vertex UU;
    UU.pos = glm::vec3(lightHigher.x, higher.y - eps, lightHigher.y);

    Material lightMat;
    unsigned int lightMatProps = EMISSIVE_FLAG;
    lightMat.color.w = reinterpret_cast<float &>(lightMatProps);

    Triangle lightTri1;
    lightTri1.V0 = LL;
    lightTri1.V1 = UL;
    lightTri1.V2 = UU;
    lightTri1.material = lightMat;

    Triangle lightTri2;
    lightTri2.V0 = LL;
    lightTri2.V1 = UU;
    lightTri2.V2 = LU;
    lightTri2.material = lightMat;

    lightSource.addTriangle(lightTri1);
    lightSource.addTriangle(lightTri2);

    lightSource.setEmissive(true);

    outObjects.push_back(walls);
    outObjects.push_back(lightSource);
}

void createSampleBoxes(std::vector<MeshObject> &outObjects) {
    MeshObject box;

    glm::vec3 lowerBox(-7.5, -7.5, -5);
    glm::vec3 higherBox(-2.5, -2.5, 0);

    std::vector<Triangle> boxTris;
    createBox(boxTris, DIFFUSE_MAGENTA, lowerBox, higherBox, ALL_FACES);

    for (Triangle &triangle: boxTris) {
        box.addTriangle(triangle);
    }

    MeshObject glasBox;
    glm::vec3 higherGlasBox(-4.5, -6.5, 2);
    glm::vec3 lowerGlasBox( -9.5, -2.5, 5);

    Material glas;
    unsigned int glasMatProperties = TRANSMISSIVE_FLAG;

    glas.color = glm::vec4(0);
    glas.color.w = reinterpret_cast<float &>(glasMatProperties);

    std::vector<Triangle> glasBoxTris;
    createBox(glasBoxTris, glas, lowerGlasBox, higherGlasBox, ALL_FACES);

    for (Triangle &triangle: glasBoxTris) {
        glasBox.addTriangle(triangle);
    }

    outObjects.push_back(box);
    outObjects.push_back(glasBox);
}

void createBox(std::vector<Triangle> &outTriangles, Material mat, glm::vec3 lower, glm::vec3 higher,
               unsigned int faceFlags) {
    std::vector<Vertex> wallVertices;

    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            for (int z = 0; z < 2; z++) {
                Vertex v;
                v.pos = (glm::vec3(1) - glm::vec3(x, y, z)) * lower + glm::vec3(x, y, z) * higher;
                wallVertices.push_back(v);
            }
        }
    }

    // left wall
    if ((faceFlags & LEFT_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 0, 1, 2, mat));
        outTriangles.push_back(createTriangle(wallVertices, 1, 3, 2, mat));
    }

    // ground
    if ((faceFlags & GROUND_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 1, 0, 4, mat));
        outTriangles.push_back(createTriangle(wallVertices, 1, 4, 5, mat));
    }

    // right wall
    if ((faceFlags & RIGHT_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 5, 4, 7, mat));
        outTriangles.push_back(createTriangle(wallVertices, 4, 6, 7, mat));
    }

    // back wall
    if ((faceFlags & BACK_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 4, 0, 6, mat));
        outTriangles.push_back(createTriangle(wallVertices, 0, 2, 6, mat));
    }

    // ceiling
    if ((faceFlags & CEIL_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 6, 2, 7, mat));
        outTriangles.push_back(createTriangle(wallVertices, 2, 3, 7, mat));
    }

    if ((faceFlags & FRONT_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 5, 1, 7, mat));
        outTriangles.push_back(createTriangle(wallVertices, 1, 3, 7, mat));
    }

}

Triangle createTriangle(std::vector<Vertex> &vertices, unsigned int index1, unsigned int index2, unsigned int index3,
                        Material mat) {
    Triangle triangle;
    triangle.V0 = vertices[index1];
    triangle.V1 = vertices[index2];
    triangle.V2 = vertices[index3];

    triangle.material = mat;

    return triangle;
}