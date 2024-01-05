#include <iostream>
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

    Material lowRoughWhite{};
    lowRoughWhite.albedo = glm::vec4(1.0f);
    lowRoughWhite.roughness = 0.0f;
    lowRoughWhite.emission = 0.0f;
    lowRoughWhite.metallic = 1.0;
    lowRoughWhite.transparency = 0.0f;

    // left wall
    MeshObject leftWall;
    /*
    leftWall.addTriangle(createTriangle(wallVertices, 1, 0, 2, RED_ROUGH));
    leftWall.addTriangle(createTriangle(wallVertices, 1, 2, 3, RED_ROUGH));
    */

    leftWall.addTriangle(createTriangle(wallVertices, 1, 0, 2, lowRoughWhite));
    leftWall.addTriangle(createTriangle(wallVertices, 1, 2, 3, lowRoughWhite));
    outObjects.push_back(leftWall);

    // ground
    MeshObject ground;
    ground.addTriangle(createTriangle(wallVertices, 0, 1, 4, BLUE_ROUGH));
    ground.addTriangle(createTriangle(wallVertices, 1, 5, 4, BLUE_ROUGH));
    outObjects.push_back(ground);

    // right wall
    MeshObject rightWall;
    rightWall.addTriangle(createTriangle(wallVertices, 4, 5, 7, GREEN_ROUGH));
    rightWall.addTriangle(createTriangle(wallVertices, 4, 7, 6, GREEN_ROUGH));
    outObjects.push_back(rightWall);

    // back wall
    MeshObject backWall;
    backWall.addTriangle(createTriangle(wallVertices, 0, 4, 6, WHITE_ROUGH));
    backWall.addTriangle(createTriangle(wallVertices, 0, 6, 2, WHITE_ROUGH));
    outObjects.push_back(backWall);

    // ceiling
    MeshObject ceiling;
    ceiling.addTriangle(createTriangle(wallVertices, 2, 6, 7, WHITE_ROUGH));
    ceiling.addTriangle(createTriangle(wallVertices, 2, 7, 3, WHITE_ROUGH));
    outObjects.push_back(ceiling);

    MeshObject lightSource;

    float xDiff = higher.x - lower.x;
    float zDiff = higher.z - lower.z;

    float percentage = 0.5;
    float eps = 1e-1;

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

    Material lightMat = WHITE_ROUGH;
    lightMat.emission = 1.0;

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

    outObjects.push_back(lightSource);
}

void createSampleBoxes(std::vector<MeshObject> &outObjects) {
    MeshObject box;

    glm::vec3 lowerBox(-7.5, -7.5, -5);
    glm::vec3 higherBox(-2.5, -2.5, 0);

    std::vector<Triangle> boxTris;
    createBox(boxTris, MAGENTA_ROUGH, lowerBox, higherBox, ALL_FACES);

    for (Triangle &triangle: boxTris) {
        box.addTriangle(triangle);
    }

    MeshObject glasBox;
    glm::vec3 lowerGlasBox( -8.5, -2.5, 4.5);
    glm::vec3 higherGlasBox(-1.5, 2.5, 5);

    std::vector<Triangle> glasBoxTris;
    createBox(glasBoxTris, YELLOW_ROUGH, lowerGlasBox, higherGlasBox, ALL_FACES);

    for (Triangle &triangle: glasBoxTris) {
        glasBox.addTriangle(triangle);
    }

    glasBox.getTransformRef().transform = glm::vec3(0, -5, 0);
    glasBox.getTransformRef().rotation = glm::vec3(0, -glm::quarter_pi<float>() / 4, 0);

    glm::vec3 lower(-1);
    glm::vec3 higher(1);

    std::vector<Triangle> transformTestBox;

    createBox(transformTestBox, CYAN_ROUGH, lower, higher, ALL_FACES);
    MeshObject transformTest;


    for (Triangle &triangle: transformTestBox) {
        transformTest.addTriangle(triangle);
    }
    Transformation &transformation = transformTest.getTransformRef();
    transformation.transform = glm::vec3(0, -6, -4);
    transformation.rotation = glm::vec3(0, 0.8 * glm::quarter_pi<float>(), 0);
    transformation.scale = glm::vec3(1);

    outObjects.push_back(glasBox);
    outObjects.push_back(transformTest);
    outObjects.push_back(box);
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

    // front
    if ((faceFlags & FRONT_WALL_FLAG) != 0) {
        outTriangles.push_back(createTriangle(wallVertices, 1, 5, 7, mat));
        outTriangles.push_back(createTriangle(wallVertices, 1, 7, 3, mat));
    }

}

Triangle createTriangle(std::vector<Vertex> &vertices, unsigned int index1, unsigned int index2, unsigned int index3,
                        Material mat) {
    Triangle triangle;
    triangle.V0 = vertices[index1];
    triangle.V1 = vertices[index2];
    triangle.V2 = vertices[index3];

    /*
    glm::vec3 edge1 = triangle.V1.pos - triangle.V0.pos;
    glm::vec3 edge2 = triangle.V2.pos - triangle.V0.pos;

    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    std::cout << "[ " << normal.x << ", " << normal.y << ", " << normal.z << "]" << std::endl;
    */

    triangle.material = mat;

    return triangle;
}