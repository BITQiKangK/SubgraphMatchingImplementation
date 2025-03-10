#pragma once

#include "alias.h"

#include <vector>
#include <unordered_map>

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

class Table : public std::vector<std::vector<std::unordered_map<VertexID, std::vector<VertexID>>>> {
public:
    Table() = default;

public:
    void table_resize(ui query_graph_size) {
        this->resize(query_graph_size);
        for (int i = 0; i < this->size(); i++) {
            (*this)[i].resize(query_graph_size);
        }
    }
};