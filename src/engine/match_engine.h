#pragma once

#include "graph/graph.h"
#include "common/aux.h"

#include <string>
#include <vector>

class MatchEngine {
public:
    MatchEngine() = default;

public:
    static void match(const Graph& data_graph, const Graph& query_graph, const std::string& filter_type_name, const std::string& order_type_name, const std::string& engine_type_name, ui max_output_embedding_num);
    
    static void reset(ui query_graph_size) {
        candidates_.clear();
        filter_order_.clear();
        matching_order_.clear();
        tree_.clear();

        candidates_.resize(query_graph_size);

        embedding_count_ = 0;
        filter_vertices_time_ = 0.;
        build_table_time_ = 0.;
        generate_query_plan_time_ = 0.;
        enumerate_time_ = 0.;
    }

private:
    static bool filter(const Graph& data_graph, const Graph& query_graph, const std::string& filter_type, std::vector<std::vector<VertexID>>& candidates, std::vector<VertexID>& filter_order, std::vector<TreeNode>& tree);

private:
    static std::vector<std::vector<VertexID>> candidates_;
    static std::vector<VertexID> filter_order_;
    static std::vector<VertexID> matching_order_;
    static std::vector<TreeNode> tree_;
    static ui embedding_count_;

private:
    static double filter_vertices_time_;
    static double order_vertices_time_;
    static double build_table_time_;
    static double generate_query_plan_time_;
    static double enumerate_time_;
};