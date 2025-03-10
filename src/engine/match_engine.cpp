#include "match_engine.h"
#include "filter/filter.h"

#include <chrono>
#include <iostream>
#include <fstream>

// static member variables
std::vector<std::vector<VertexID>> MatchEngine::candidates_;
std::vector<VertexID> MatchEngine::filter_order_;
std::vector<VertexID> MatchEngine::matching_order_;
std::vector<TreeNode> MatchEngine::tree_;
ui MatchEngine::embedding_count_;

double MatchEngine::filter_vertices_time_;
double MatchEngine::build_table_time_;
double MatchEngine::generate_query_plan_time_;
double MatchEngine::enumerate_time_;

void MatchEngine::match(const Graph& data_graph, const Graph& query_graph, const std::string& filter_type, const std::string& order_type, const std::string& engine_type, ui max_embedding_num) {
    // Reset the state of the MatchEngine
    reset(query_graph.get_vertices_count());

    // Filter vertices based on filter type
    auto start_filter_time = std::chrono::high_resolution_clock::now();
    Filter::filter(data_graph, query_graph, filter_type.c_str(), candidates_, filter_order_, tree_);
    auto end_filter_time = std::chrono::high_resolution_clock::now();
    filter_vertices_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_filter_time - start_filter_time).count();
    std::cout << "Filter vertices time: " << filter_vertices_time_ / 1000 << " s" << std::endl;

#ifdef DEBUGMODE
    for (int i = 0; i < candidates_.size(); i++) {
        std::cout << i << " Candidates size: " << candidates_[i].size() << std::endl;
    }
#endif

#ifdef FILTERLOG
    for (int i = 0; i < candidates_.size(); i++) {
        std::cout << "Filter candidates size: " << candidates_[i].size() << std::endl;
    }
#endif

    // // Order vertices based on order type
    // auto start_order_time = std::chrono::high_resolution_clock::now();
    // order(data_graph, query_graph, order_type, candidates_, matching_order_);
    // auto end_order_time = std::chrono::high_resolution_clock::now();
    // order_vertices_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end_order_time - start_order_time).count();

    // // Build table

}