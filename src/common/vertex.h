#pragma once

#include "alias.h"

#include <vector>

// 节点类型，包含节点ID和节点标签，所连接边的数组
struct Vertex {
    Vertex() {}
    Vertex(VertexLabel label) : label(label) {}

    VertexLabel label = 0;
    std::vector<VertexID> neighbors_id; // 所连接边的数组
};
