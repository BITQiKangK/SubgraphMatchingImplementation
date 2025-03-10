#include "build_table.h"

void BuildTable::build_table(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>> candidates, Table& table) {
    ui query_vertex_num = query_graph.get_vertices_count();

    table.table_resize(query_vertex_num);
    std::vector<VertexID> build_table_order(query_vertex_num);
    for (ui i = 0; i < query_vertex_num; i++) {
        build_table_order[i] = i;
    }

    std::sort(build_table_order.begin(), build_table_order.end(), [query_graph](VertexID l, VertexID r) {
        if (query_graph.get_vertex_degree(l) == query_graph.get_vertex_degree(r)) {
            return l < r;
        }
        return query_graph.get_vertex_degree(l) > query_graph.get_vertex_degree(r);
    });

    for (auto u : build_table_order) {
        const auto& u_nbrs = query_graph.get_vertex_neighbors(u);

        for (auto u_nbr : u_nbrs) {
            for (auto v : candidates[u]) {
                std::vector<VertexID> temp_intersection;
                const auto& v_nbrs = data_graph.get_vertex_neighbors(v);
                std::set_intersection(v_nbrs.begin(), v_nbrs.end(),
                                      candidates[u_nbr].begin(), candidates[u_nbr].end(),
                                      std::back_inserter(temp_intersection));
                if (temp_intersection.size() != 0) {
                    table[u][u_nbr][v] = std::move(temp_intersection);
                    for (auto v_nbr : table[u][u_nbr][v]) {
                        table[u_nbr][u][v_nbr].push_back(v);
                    }
                }
            }
        }
    }

}