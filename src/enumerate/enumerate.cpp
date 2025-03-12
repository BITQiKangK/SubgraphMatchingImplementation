#include "enumerate.h"

#include <string.h>
#include <iostream>

const char* ENUMERATE_TYPE_NAME[] = {"UNDEFINED", "LFTJ", "EXPLORE"};

const char* enumerate_type_to_string(EnumerateType enumerate_type) {
    if (enumerate_type > EnumerateType::UNDEFINED && enumerate_type < EnumerateType::MAXTYPE) {
        return ENUMERATE_TYPE_NAME[static_cast<int>(enumerate_type)];
    }
    
    return "UNKNOWN"; 
}

EnumerateType enumerate_type_from_string(const char* enumerate_type_name) {
    for (int i = 1; i < sizeof(ENUMERATE_TYPE_NAME) / sizeof(ENUMERATE_TYPE_NAME[0]); i++) {
        if (0 == strcmp(ENUMERATE_TYPE_NAME[i], enumerate_type_name)) {
            return (EnumerateType)i;
        }
    }
    return EnumerateType::UNDEFINED;
}

size_t Enumerate::enumerate(const Graph& data_graph, const Graph& query_graph, const char* enumerate_type_name, const Table& table, const std::vector<std::vector<VertexID>>& candidates,
                          const std::vector<VertexID>& order,const std::vector<VertexID>& pivot, size_t& call_count) {
    EnumerateType enumerate_type = enumerate_type_from_string(enumerate_type_name);

    switch (enumerate_type) {
        case EnumerateType::EXPLORE:
            return explore_graph(data_graph, query_graph, table, candidates, order, pivot, call_count);
        case EnumerateType::LFTJ :
            return LFTJ(data_graph, query_graph, table, candidates, order, call_count);
        default:
            // Handle unknown order type
            std::cerr << "Unknown enumerate type: " << enumerate_type_name << std::endl;
    }
    return 0;
}


// =============================
// Below are enumerating methods
// =============================

size_t Enumerate::explore_graph(const Graph& data_graph, const Graph& query_graph, const Table& table, const std::vector<std::vector<VertexID>>& candidates, const std::vector<VertexID>& order, const std::vector<VertexID>& pivot, size_t& call_count) {
    std::vector<std::vector<VertexID>> bn;
    generate_bn(query_graph, order, pivot, bn);
    ui query_vertex_num = query_graph.get_vertices_count();

    std::vector<VertexID> current_candidate_idx(query_vertex_num);
    std::vector<VertexID> embedding(query_vertex_num);
    std::vector<std::vector<VertexID>> valid_candidates(query_vertex_num);
    std::vector<bool> visited_vertices(data_graph.get_vertices_count(), false);

    size_t embedding_cnt = 0;
    int cur_depth = 0;
    int max_depth = query_graph.get_vertices_count();
    VertexID start_u_id = order[0];

    for (auto start_v_id : candidates[start_u_id]) {
        embedding[start_u_id] = start_v_id;
        visited_vertices[start_v_id] = true;
        cur_depth++;
        current_candidate_idx[cur_depth] = 0;
        call_count += 1;
        generate_valid_candidates(data_graph, order[cur_depth], pivot[cur_depth], table, embedding, visited_vertices, bn[cur_depth], candidates, valid_candidates[cur_depth]);

        while (cur_depth >= 1) {
            if (current_candidate_idx[cur_depth] < valid_candidates[cur_depth].size()) {
                VertexID u_id = order[cur_depth];
                VertexID v_id = valid_candidates[cur_depth][current_candidate_idx[cur_depth]];
                current_candidate_idx[cur_depth] += 1;
    
                embedding[u_id] = v_id;
                visited_vertices[v_id] = true;
    
                if (cur_depth == max_depth - 1) {
                    embedding_cnt += 1;
                    visited_vertices[v_id] = false;
                } else {
                    call_count += 1;
                    cur_depth += 1;
                    current_candidate_idx[cur_depth] = 0;
                    generate_valid_candidates(data_graph, order[cur_depth], pivot[cur_depth], table, embedding, visited_vertices, bn[cur_depth], candidates, valid_candidates[cur_depth]);
                }
            } else {
                cur_depth--;
                visited_vertices[embedding[order[cur_depth]]] = false;
            }
        }   
    }

    return embedding_cnt;
}

size_t Enumerate::LFTJ(const Graph& data_graph, const Graph& query_graph, const Table& table, const std::vector<std::vector<VertexID>>& candidates, const std::vector<VertexID>& order, size_t& call_count) {
    std::vector<std::vector<VertexID>> bn;
    generate_bn(query_graph, order, bn);
    ui query_vertex_num = query_graph.get_vertices_count();

    std::vector<VertexID> current_candidate_idx(query_vertex_num);
    std::vector<VertexID> embedding(query_vertex_num);
    std::vector<std::vector<VertexID>> valid_candidates(query_vertex_num);
    std::vector<bool> visited_vertices(data_graph.get_vertices_count(), false);
    
    size_t embedding_cnt = 0;
    int cur_depth = 0;
    int max_depth = query_vertex_num;
    VertexID start_u_id = order[0];

    for (auto start_v_id : candidates[start_u_id]) {
        embedding[start_u_id] = start_v_id;
        visited_vertices[start_v_id] = true;
        cur_depth++;
        current_candidate_idx[cur_depth] = 0;
        call_count += 1;
        generate_valid_candidates(data_graph, order[cur_depth], table, embedding, visited_vertices, bn[cur_depth], candidates, valid_candidates[cur_depth]);

        while (cur_depth >= 1) {
            if (current_candidate_idx[cur_depth] < valid_candidates[cur_depth].size()) {
                VertexID u_id = order[cur_depth];
                VertexID v_id = valid_candidates[cur_depth][current_candidate_idx[cur_depth]];
    
                if (visited_vertices[v_id]) {
                    current_candidate_idx[cur_depth] += 1;
                    continue;
                }
    
                embedding[u_id] = v_id;
                visited_vertices[v_id] = true;
                current_candidate_idx[cur_depth] += 1;
                
                if (cur_depth == max_depth - 1) {
                    embedding_cnt += 1;
                    visited_vertices[v_id] = false;
                } else {
                    call_count += 1;
                    cur_depth += 1;
                    current_candidate_idx[cur_depth] = 0;
                    generate_valid_candidates(data_graph, order[cur_depth], table, embedding, visited_vertices, bn[cur_depth], candidates, valid_candidates[cur_depth]);
                }
            } else {
                cur_depth--;
                visited_vertices[embedding[order[cur_depth]]] = false;
            }
        }
    }

    return embedding_cnt;
}

// ==============================================
// Below are generating valid candidates methods
// ==============================================

void Enumerate::generate_valid_candidates(const Graph& data_graph, VertexID u_id, VertexID pivot_id, const Table& table, const std::vector<VertexID>& embedding,
                                          const std::vector<bool>& visited_vertices, const std::vector<VertexID>& cur_depth_bn, const std::vector<std::vector<VertexID>>& candidates,
                                          std::vector<VertexID>& cur_depth_valid_candidates) {
    cur_depth_valid_candidates.clear();
    if (!table[pivot_id][u_id].count(embedding[pivot_id])) return;
    const auto& v_ids = table[pivot_id][u_id].at(embedding[pivot_id]);

    if (cur_depth_bn.size() == 0) {
        for (auto v_id : v_ids) {
            if (!visited_vertices[v_id])
                cur_depth_valid_candidates.push_back(v_id);
        }
    } else {
        for (auto v_id : v_ids) {
            if (!visited_vertices[v_id]) {
                bool valid = true;

                for (ui i = 0; i < cur_depth_bn.size(); i++) {
                    VertexID u_bn = cur_depth_bn[i];
                    VertexID v_bn = embedding[u_bn];

                    if (!data_graph.check_edge_existence(v_id, v_bn)) {
                        valid = false;
                        break;
                    }
                }

                if (valid) {
                    cur_depth_valid_candidates.push_back(v_id);
                }    
            }
        }
    }
}

void Enumerate::generate_valid_candidates(const Graph& data_graph, VertexID u_id, const Table& table, const std::vector<VertexID>& embedding, const std::vector<bool>& visited_vertices,
                                          const std::vector<VertexID>& cur_depth_bn, const std::vector<std::vector<VertexID>>& candidates,
                                          std::vector<VertexID>& cur_depth_valid_candidates) {
    cur_depth_valid_candidates.clear();
    if (!table[cur_depth_bn[0]][u_id].count(embedding[cur_depth_bn[0]])) return;
    cur_depth_valid_candidates = table[cur_depth_bn[0]][u_id].at(embedding[cur_depth_bn[0]]);

    for (ui i = 1; i < cur_depth_bn.size(); i++) {
        VertexID current_bn_id = cur_depth_bn[i];
        if (!table[current_bn_id][u_id].count(embedding[current_bn_id])) {
            cur_depth_valid_candidates.clear();
            return;
        }
        std::vector<VertexID> current_candidates = table[current_bn_id][u_id].at(embedding[current_bn_id]);
        std::vector<VertexID> temp_vec;

        std::set_intersection(current_candidates.begin(), current_candidates.end(),
                              cur_depth_valid_candidates.begin(), cur_depth_valid_candidates.end(),
                              std::back_inserter(temp_vec));
        std::swap(cur_depth_valid_candidates, temp_vec);
    }                                     
}

// ===============================
// Below are generating bn methods
// ===============================

void Enumerate::generate_bn(const Graph& query_graph, const std::vector<VertexID>& order, std::vector<std::vector<VertexID>>& bn) {
    ui query_vertices_num = query_graph.get_vertices_count();
    bn.resize(query_vertices_num);

    bool visited_vertices[query_vertices_num] = {};
    visited_vertices[order[0]] = true;
    for (ui i = 1; i < query_vertices_num; i++) {
        VertexID vertex = order[i];

        const auto& nbrs = query_graph.get_vertex_neighbors(vertex);
        for (auto nbr : nbrs) {
            if (visited_vertices[nbr]) {
                bn[i].push_back(nbr);
            }
        }
        visited_vertices[vertex] = true;
    }
}

void Enumerate::generate_bn(const Graph& query_graph, const std::vector<VertexID>& order, const std::vector<VertexID>& pivot, std::vector<std::vector<VertexID>>& bn) {
    ui query_vertices_num = query_graph.get_vertices_count();
    bn.resize(query_vertices_num);

    bool visited_vertices[query_vertices_num] = {};
    visited_vertices[order[0]] = true;
    for (ui i = 1; i < query_vertices_num; i++) {
        VertexID vertex = order[i];

        const auto& nbrs = query_graph.get_vertex_neighbors(vertex);
        for (auto nbr : nbrs) {
            if (visited_vertices[nbr] && nbr != pivot[i]) {
                bn[i].push_back(nbr);
            }
        }
        visited_vertices[vertex] = true;
    }
}