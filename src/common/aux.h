#pragma once

#include "alias.h"

#include <vector>

class TreeNode {
public:
    TreeNode() : id_(INVALID_VERTEX_ID), parent_(INVALID_VERTEX_ID) {};
public:
    VertexID id_;
    VertexID parent_;
    ui level_;
    std::vector<VertexID> under_level_;
    std::vector<VertexID> children_;  
    std::vector<VertexID> bn_;
    std::vector<VertexID> fn_;
    size_t estimated_embeddings_num_;
};