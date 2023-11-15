#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtx/euler_angles.hpp>

typedef struct transform_s {
    glm::vec3 transform { glm::vec3(0) };
    glm::vec3 rotation { glm::vec3(0) };
    glm::vec3 scale { glm::vec3(1) };

    glm::mat4x4 getMatrix() {
        glm::mat4x4 res(1);

        res *= glm::translate(glm::mat4(1), transform);
        res *= glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
        res *= glm::scale(glm::mat4(1), scale);

        return res;
    }
} Transformation;