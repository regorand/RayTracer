#pragma once

#include "scene/MeshObject.h"

const unsigned int LEFT_WALL_FLAG = 1 << 0;
const unsigned int RIGHT_WALL_FLAG = 1 << 1;
const unsigned int GROUND_WALL_FLAG = 1 << 2;
const unsigned int CEIL_WALL_FLAG = 1 << 3;
const unsigned int BACK_WALL_FLAG = 1 << 4;
const unsigned int FRONT_WALL_FLAG = 1 << 5;

const unsigned int ALL_FACES =
        LEFT_WALL_FLAG | RIGHT_WALL_FLAG | GROUND_WALL_FLAG | CEIL_WALL_FLAG | BACK_WALL_FLAG | FRONT_WALL_FLAG;
const unsigned int ALL_EXCEPT_FRONT =
        LEFT_WALL_FLAG | RIGHT_WALL_FLAG | GROUND_WALL_FLAG | CEIL_WALL_FLAG | BACK_WALL_FLAG;

const static Material DIFFUSE_RED = Material(glm::vec4(1, 0.2, 0.2, 0.0f));
const static Material DIFFUSE_GREEN = Material(glm::vec4(0.2, 1, 0.2, 0.0f));
const static Material DIFFUSE_BLUE = Material(glm::vec4(0.2, 0.2, 1, 0.0f));

const static Material DIFFUSE_YELLOW = Material(glm::vec4(1, 1, 0.2, 0.0f));
const static Material DIFFUSE_MAGENTA = Material(glm::vec4(1, 0.2, 1, 0.0f));
const static Material DIFFUSE_CYAN = Material(glm::vec4(0.2, 1, 1, 0.0f));

const static Material DIFFUSE_WHITE = Material(glm::vec4(1, 1, 1, 0.0f));

void createCornellBox(std::vector<MeshObject> &outObjects);

void createSampleBoxes(std::vector<MeshObject> &outObjects);

void
createBox(std::vector<Triangle> &outTriangles, Material mat, glm::vec3 lower, glm::vec3 higher, unsigned int faceFlags);

Triangle createTriangle(std::vector<Vertex> &vertices, unsigned int index1, unsigned int index2, unsigned int index3,
                        Material mat);