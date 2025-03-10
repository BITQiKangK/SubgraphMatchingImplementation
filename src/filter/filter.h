#pragma once

#include "graph/graph.h"
#include "common/aux.h"

enum class FilterType {
    UNDEFINED,
    LDF,
    NLF,
    DPiso,
    MAXTYPE,
};

const char* filter_type_to_string(FilterType filter_type);
FilterType filter_type_from_string(const char* filter_type_name);

class Filter {
public:
    static bool filter(const Graph& data_graph, const Graph& query_graph, const char* filter_type_name, std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree);

private:
    // filter type
    static bool LDFFilter(const Graph& data_graph, const Graph &query_graph, std::vector<std::vector<VertexID>>& candidates);
    static bool NLFFilter(const Graph& data_graph, const Graph &query_graph, std::vector<std::vector<VertexID>>& candidates);
    static bool DPisoFilter(const Graph& data_graph, const Graph &query_graph, std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree);

private:
    // utility functions
    static void compute_candidate_with_LDF(const Graph& data_graph, const Graph& query_graph, VertexID query_vertex_id, std::vector<VertexID>& candidate);
    static void compute_candidate_with_NLF(const Graph& data_graph, const Graph& query_graph, VertexID query_vertex_id, std::vector<VertexID>& candidate);
    static void prune_candidates(const Graph& data_graph, const Graph& query_graph, VertexID query_vertex_id, const std::vector<VertexID>& neighbors_id, std::vector<std::vector<VertexID>>& candidates);
    static bool is_candidate_set_valid(const std::vector<std::vector<VertexID>>& candidates);
};

class FilterPlan {
private:
    // generate filter plan
    static void generate_DPiso_filter_plan(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree);

private:
    // select first vertex
    static VertexID select_DPiso_start_vertex(const Graph& data_graph, const Graph& query_graph, const std::vector<std::vector<VertexID>>& candidates);
    
    friend class Filter;
};