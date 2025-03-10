#pragma once

#include "graph/graph.h"
#include "common/aux.h"

class GraphOperations {
public:
    static void bfs_traversal(const Graph& graph, VertexID start_vertex_id, std::vector<VertexID>& order, std::vector<TreeNode>& tree);
};