#pragma once

#include "graph/graph.h"
#include "common/aux.h"

#include <vector>

enum class EnumerateType {
    UNDEFINED,
    LFTJ,
    EXPLORE,
    MAXTYPE,
};

const char* enumerate_type_to_string(EnumerateType enumerate_type);
EnumerateType enumerate_type_from_string(const char* enumerate_type_name);

class Enumerate {
public:
    static size_t enumerate(const Graph& data_graph, const Graph& query_graph, const char* enumerate_type_name, const Table& table, const std::vector<std::vector<VertexID>>& candidates,
                          const std::vector<VertexID>& order, const std::vector<VertexID>& pivot, size_t& call_count);

private:
    static size_t explore_graph(const Graph& data_graph, const Graph& query_graph, const Table& table, const std::vector<std::vector<VertexID>>& candidates, const std::vector<VertexID>& order, const std::vector<VertexID>& pivot, size_t& call_count);
    static size_t LFTJ(const Graph& data_graph, const Graph& query_graph, const Table& table,
                     const std::vector<std::vector<VertexID>>& candidates, const std::vector<VertexID>& order,
                     size_t& call_count);

private:
    static void generate_bn(const Graph& query_graph, const std::vector<VertexID>& order, const std::vector<VertexID>& piovt, std::vector<std::vector<VertexID>>& bn);
    static void generate_bn(const Graph& query_graph, const std::vector<VertexID>& order, std::vector<std::vector<VertexID>>& bn);

private:
    static void generate_valid_candidates(const Graph& data_graph, VertexID u_id, VertexID pivot_id, const Table& table, const std::vector<VertexID>& embedding,
                                          const std::vector<bool>& visited_vertices, const std::vector<VertexID>& cur_depth_bn, const std::vector<std::vector<VertexID>>& candidates,
                                          std::vector<VertexID>& cur_depth_valid_candidates);
};