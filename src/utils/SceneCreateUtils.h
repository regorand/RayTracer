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

const static MaterialOld RED_ROUGH = {glm::vec4(1, 0.1, 0.1, 1.0f),
                                      glm::vec4(0),
                                      1.0,
                                      1.0,
                                      0.0,
                                      0.0};

const static MaterialOld GREEN_ROUGH = {glm::vec4(0.1, 1.0, 0.1, 1.0f),
                                        glm::vec4(0),
                                        1.0,
                                        1.0,
                                        0.0,
                                        0.0};

const static MaterialOld BLUE_ROUGH = {glm::vec4(0.1, 0.1, 1.0, 1.0f),
                                       glm::vec4(0),
                                       1.0,
                                       1.0,
                                       0.0,
                                       0.0};

const static MaterialOld YELLOW_ROUGH = {glm::vec4(1.0, 1.0, 0.1, 1.0f),
                                         glm::vec4(0),
                                         1.0,
                                         1.0,
                                         0.0,
                                         0.0};

const static MaterialOld MAGENTA_ROUGH = {glm::vec4(1.0, 0.1, 1.0, 1.0f),
                                          glm::vec4(0),
                                          1.0,
                                          1.0,
                                          0.0,
                                          0.0};

const static MaterialOld CYAN_ROUGH = {glm::vec4(0.1, 1.0, 1.0, 1.0f),
                                       glm::vec4(0),
                                       1.0,
                                       1.0,
                                       0.0,
                                       0.0};

const static MaterialOld WHITE_ROUGH = {glm::vec4(1.0, 1.0, 1.0, 1.0f),
                                        glm::vec4(0),
                                        1.0,
                                        1.0,
                                        0.0,
                                        0.0};


void createCornellBox(std::vector<MeshObject> &outObjects);

void createSampleBoxes(std::vector<MeshObject> &outObjects);

void
createBox(std::vector<Triangle> &outTriangles, MaterialOld mat, glm::vec3 lower, glm::vec3 higher, unsigned int faceFlags);

Triangle createTriangle(std::vector<VertexOld> &vertices, unsigned int index1, unsigned int index2, unsigned int index3,
                        MaterialOld mat);