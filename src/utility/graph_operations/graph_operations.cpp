#include "graph_operations.h"

#include <queue>

void GraphOperations::bfs_traversal(const Graph& graph, VertexID start_vertex_id, std::vector<VertexID>& order, std::vector<TreeNode>& tree) {
    tree.resize(graph.get_vertices_count());
    ui vertex_num = graph.get_vertices_count();

    std::queue<VertexID> queue;
    bool visited[vertex_num] = {};

    queue.push(start_vertex_id);
    visited[start_vertex_id] = true;

    ui level = 0;
    while (!queue.empty()) {
        ui queue_size = queue.size();
        for (ui i = 0; i < queue_size; i++) {
            VertexID cur_vertex_id = queue.front();
            queue.pop();

            order.push_back(cur_vertex_id);
            tree[cur_vertex_id].level_ = level;

            const auto& neighbors_id = graph.get_vertex_neighbors(cur_vertex_id);
            for (auto nbr_id : neighbors_id) {
                if (!visited[nbr_id]) {
                    visited[nbr_id] = true;
                    tree[nbr_id].parent_ = cur_vertex_id;
                    tree[cur_vertex_id].children_.push_back(nbr_id);
                    queue.push(nbr_id);
                }
            }
        }
        level++;
    }
}