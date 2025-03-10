#include "filter.h"
#include "utility/graph_operations/graph_operations.h"

#include <string.h>
#include <iostream>
#include <algorithm>
#include <set>

const char* FILTER_TYPE_NAME[] = {"UNDEFINED", "LDF", "NLF", "DPiso"};

const char* filter_type_to_string(FilterType filter_type) {
    if (filter_type > FilterType::UNDEFINED && filter_type < FilterType::MAXTYPE) {
        return FILTER_TYPE_NAME[static_cast<int>(filter_type)];
    }
    return "UNKNOWN";
}

FilterType filter_type_from_string(const char* filter_type_name) {
    for (unsigned int i = 1; i < sizeof(FILTER_TYPE_NAME) / sizeof(FILTER_TYPE_NAME[0]); i++) {
        if (0 == strcmp(FILTER_TYPE_NAME[i], filter_type_name)) {
            return (FilterType)i;
        }
    }
    return FilterType::UNDEFINED;
}

// Filter Methods

bool Filter::filter(const Graph& data_graph, const Graph& query_graph, const char* filter_type_name, std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree) {
    FilterType filter_type = filter_type_from_string(filter_type_name);
    switch (filter_type) {
        case FilterType::LDF:
            return LDFFilter(data_graph, query_graph, candidates);
        case FilterType::NLF:
            return NLFFilter(data_graph, query_graph, candidates);
        case FilterType::DPiso:
            return DPisoFilter(data_graph, query_graph, candidates, filter_order, tree);
        default:
            // Handle unknown filter type
            std::cerr << "Unknown filter type: " << filter_type_name << std::endl;
            return false;
    }
}


// -----------------------------------
// Below Are Filter Methods
// ------------------------------------

bool Filter::LDFFilter(const Graph& data_graph, const Graph &query_graph, std::vector<std::vector<VertexID>>& candidates) {
    for (VertexID query_vertex_id = 0; query_vertex_id < query_graph.get_vertices_count(); query_vertex_id++) {
        compute_candidate_with_LDF(data_graph, query_graph, query_vertex_id, candidates[query_vertex_id]);
        if (candidates[query_vertex_id].size() == 0) {
            return false;
        }
    }
    return true;
}

bool Filter::NLFFilter(const Graph& data_graph, const Graph &query_graph, std::vector<std::vector<VertexID>>& candidates) {
    for (VertexID query_vertex_id = 0; query_vertex_id < query_graph.get_vertices_count(); query_vertex_id++) {
        compute_candidate_with_NLF(data_graph, query_graph, query_vertex_id, candidates[query_vertex_id]);
        if (candidates[query_vertex_id].size() == 0) {
            return false;
        }
    }
    return true;
}

bool Filter::DPisoFilter(const Graph& data_graph, const Graph &query_graph, std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree) {
    if (!LDFFilter(data_graph, query_graph, candidates)) {
        return false;
    }

    FilterPlan::generate_DPiso_filter_plan(data_graph, query_graph, candidates, filter_order, tree);

    ui query_vertices_count = query_graph.get_vertices_count();

    for (ui k = 0; k < 3; k++) {
        if (k % 2 == 0) {
            // forward filter along the tree
            for (int i = 1; i < query_vertices_count; i++) {
                VertexID query_vertex_id = filter_order[i];
                const auto& node = tree[query_vertex_id];
                prune_candidates(data_graph, query_graph, query_vertex_id, node.bn_, candidates);
            }
        } else {
            // backward filter along the tree
            for (int i = query_vertices_count - 2; i >= 0; i--) {
                VertexID query_vertex_id = filter_order[i];
                const auto& node = tree[query_vertex_id];
                prune_candidates(data_graph, query_graph, query_vertex_id, node.fn_, candidates);
            }
        }
    }

    return is_candidate_set_valid(candidates);
}


// -----------------------------------
// Below Are Helper Methods
// ------------------------------------

void Filter::compute_candidate_with_LDF(const Graph& data_graph, const Graph& query_graph, VertexID query_vertex_id, std::vector<VertexID>& candidate) {
    VertexLabel query_vertex_label = query_graph.get_vertex_label(query_vertex_id);
    ui query_vertex_degree = query_graph.get_vertex_degree(query_vertex_id);

    auto& data_vertices_id = data_graph.get_vertices_by_label(query_vertex_label);

    for (auto& data_vertex_id : data_vertices_id) {
        if (data_graph.get_vertex_degree(data_vertex_id) >= query_vertex_degree) {
            candidate.push_back(data_vertex_id);
        }
    }
}

void Filter::compute_candidate_with_NLF(const Graph& data_graph, const Graph &query_graph, VertexID query_vertex_id, std::vector<VertexID>& candidate) {
    VertexLabel query_vertex_label = query_graph.get_vertex_label(query_vertex_id);
    ui query_vertex_degree = query_graph.get_vertex_degree(query_vertex_id);
#if OPTIMIZED_LABELED_GRAPH == 1
    const auto& query_vertex_neighbors_label2count_map = query_graph.get_vertex_neighbors_label2count_map(query_vertex_id);
#endif

    auto& data_vertices_id = data_graph.get_vertices_by_label(query_vertex_label);

    for (auto& data_vertex_id : data_vertices_id) {
        if (data_graph.get_vertex_degree(data_vertex_id) >= query_vertex_degree) {
#if OPTIMIZED_LABELED_GRAPH == 1
            const auto& data_vertex_neighbors_label2count_map = data_graph.get_vertex_neighbors_label2count_map(data_vertex_id);
            if (data_vertex_neighbors_label2count_map.size() < query_vertex_neighbors_label2count_map.size()) {
                continue;
            }
            bool flag = true;
            for (const auto& query_vertex_neighbors_label2count : query_vertex_neighbors_label2count_map) {
                if (data_vertex_neighbors_label2count_map.count(query_vertex_neighbors_label2count.first) == 0 ||
                    data_vertex_neighbors_label2count_map.at(query_vertex_neighbors_label2count.first) < query_vertex_neighbors_label2count.second) {
                    flag = false;
                    break;
                }
            }
            if (flag) {
                candidate.push_back(data_vertex_id);
            }
#else
            candidate.push_back(data_vertex_id);
#endif
        }
    }
}

void Filter::prune_candidates(const Graph& data_graph, const Graph& query_graph, VertexID query_vertex_id, const std::vector<VertexID>& neighbors_id, std::vector<std::vector<VertexID>>& candidates) {
    VertexLabel query_vertex_label = query_graph.get_vertex_label(query_vertex_id);
    ui query_vertex_degree = query_graph.get_vertex_degree(query_vertex_id);
    
    // 本质是candidate_set 与 （一堆集合并集的交集） 的交集
    std::set<VertexID> pruning_candidates;
    pruning_candidates.insert(candidates[query_vertex_id].begin(), candidates[query_vertex_id].end());
    
    for (auto neighbor_id : neighbors_id) {
        // 不同neighbor_id之间的结果取交集
        std::set<VertexID> temp_candidates;
        for (auto neighbor_data_id : candidates[neighbor_id]) {
            // 一个neighbor_id的candidate内部取并集
            const auto& query_candidates_id = data_graph.get_vertex_neighbors(neighbor_data_id);
            for (VertexID query_candidate_id : query_candidates_id) {
                if (data_graph.get_vertex_label(query_candidate_id) == query_vertex_label &&
                    data_graph.get_vertex_degree(query_candidate_id) >= query_vertex_degree &&
                    pruning_candidates.count(query_candidate_id)) {
                    temp_candidates.insert(query_candidate_id);
                }
            }
        }

        // 取交集部分
        std::set<VertexID> temp_intersection_result;
        std::set_intersection(pruning_candidates.begin(), pruning_candidates.end(),
                              temp_candidates.begin(), temp_candidates.end(),
                              std::inserter(temp_intersection_result, temp_intersection_result.begin()));
        std::swap(pruning_candidates, temp_intersection_result);
    }

    std::vector<VertexID> temp_vec;
    for (auto it : pruning_candidates) {
        temp_vec.push_back(it);
    }
    std::swap(candidates[query_vertex_id], temp_vec);
}

bool Filter::is_candidate_set_valid(const std::vector<std::vector<VertexID>>& candidates) {
    for (auto& candidate : candidates) {
        if (candidate.size() == 0) {
            return false;
        }
    }
    return true;
}


// -----------------------------------
// Below Are Filter Plan Methods
// ------------------------------------
void FilterPlan::generate_DPiso_filter_plan(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree) {
    VertexID start_vertex_id = select_DPiso_start_vertex(data_graph, query_graph, candidates);
    GraphOperations::bfs_traversal(query_graph, start_vertex_id, filter_order, tree);

    ui query_vertices_count = query_graph.get_vertices_count();

    std::vector<ui> order_index(query_vertices_count);
    for (ui i = 0; i < query_vertices_count; i++) {
        VertexID query_vertex_id = filter_order[i];
        order_index[query_vertex_id] = i;
    }

    for (ui i = 0; i < query_vertices_count; i++) {
        VertexID query_vertex_id = filter_order[i];

        const auto& nbrs_id = query_graph.get_vertex_neighbors(query_vertex_id);

        for (auto nbr_id : nbrs_id) {
            if (order_index[nbr_id] < order_index[query_vertex_id]) {
                tree[query_vertex_id].bn_.push_back(nbr_id);
            }
            else {
                tree[query_vertex_id].fn_.push_back(nbr_id);
            }
        }
    }

}

VertexID FilterPlan::select_DPiso_start_vertex(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates) {
    double min_score = data_graph.get_vertices_count();
    VertexID start_vertex_id = 0;

    for (VertexID vertex_id = 0; vertex_id < query_graph.get_vertices_count(); vertex_id++) {
        ui degree = query_graph.get_vertex_degree(vertex_id);
        if (degree <= 1) {
            continue;
        }

        double cur_score = candidates[vertex_id].size() / degree;
        if (cur_score < min_score) {
            min_score = cur_score;
            start_vertex_id = vertex_id;
        }
    }

    return start_vertex_id;
}
