#pragma once

#include <vector>
#include "glm/ext/matrix_transform.hpp"

enum class NodeType {
    INNER,
    MESH,
    CAMERA
};

typedef struct graph_node_s {
    NodeType type {};
    glm::mat4 transform {};
    std::vector<graph_node_s> children {};

    int index = -1; // indexes into Mesh or Camera arrays

    graph_node_s()
        : type(NodeType::INNER), transform({1}), children(), index(-1)
    {}

    void clear() {
        type = NodeType::INNER;
        transform = glm::mat4(1);
        children.clear();
        index = -1;
    }
} GraphNode;