#include "match_engine.h"
#include "filter/filter.h"
#include "build_table/build_table.h"
#include "order/order.h"
#include "enumerate/enumerate.h"

#include <chrono>
#include <iostream>
#include <fstream>

#define NANOSECTOSEC(elapsed_time) ((elapsed_time)/(double)1000000000)

// static member variables
std::vector<std::vector<VertexID>> MatchEngine::candidates_;
std::vector<VertexID> MatchEngine::filter_order_;
std::vector<VertexID> MatchEngine::matching_order_;
std::vector<VertexID> MatchEngine::pivot_;
std::vector<TreeNode> MatchEngine::tree_;
Table MatchEngine::table_;
ui MatchEngine::embedding_count_;

double MatchEngine::filter_vertices_time_;
double MatchEngine::build_table_time_;
double MatchEngine::order_vertices_time_;
double MatchEngine::enumerate_time_;

void MatchEngine::match(const Graph& data_graph, const Graph& query_graph, const std::string& filter_type, const std::string& order_type, const std::string& engine_type, ui max_embedding_num) {
    // Reset the state of the MatchEngine
    reset(query_graph.get_vertices_count());

    // Filter vertices based on filter type
    std::cout << "1. Filter." << std::endl;
    std::cout << "1.1 filter." << std::endl;
    auto start_filter_time = std::chrono::high_resolution_clock::now();
    Filter::filter(data_graph, query_graph, filter_type.c_str(), candidates_, filter_order_, tree_);
    auto end_filter_time = std::chrono::high_resolution_clock::now();
    filter_vertices_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end_filter_time - start_filter_time).count();
    std::cout << "Filter vertices time: " << NANOSECTOSEC(filter_vertices_time_) << " s" << std::endl;

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

    // Build Tables Here
    std::cout << "1.2 build tables" << std::endl;
    auto start_build_table_time = std::chrono::high_resolution_clock::now();
    BuildTable::build_table(data_graph, query_graph, candidates_, table_);
    auto end_build_table_time = std::chrono::high_resolution_clock::now();
    build_table_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end_build_table_time - start_build_table_time).count();
    std::cout << "Build table time: " << NANOSECTOSEC(build_table_time_) << " s" << std::endl;
    std::cout << "Table size: " << sizeof(table_) << " B" << std::endl;

    // Order vertices based on order type
    std::cout << std::endl;
    std::cout << "2. Order." << std::endl;
    auto start_order_time = std::chrono::high_resolution_clock::now();
    Order::order(data_graph, query_graph, order_type.c_str(), candidates_, matching_order_, pivot_);
    auto end_order_time = std::chrono::high_resolution_clock::now();
    order_vertices_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end_order_time - start_order_time).count();
    std::cout << "Order vertices time: " << NANOSECTOSEC(order_vertices_time_) << " s" << std::endl;

#ifdef DEBUGMODE
    std::cout << "Matching Order: " << std::endl;
    for (int i = 0; i < matching_order_.size(); i++) {
        std::cout << matching_order_[i] << " ";
    }
    std::cout << std::endl;
#endif

    // Enumerate
    std::cout << std::endl;
    std::cout << "3. Enumerate" << std::endl;
    size_t call_count = 0;
    
    auto start_enumerate_time = std::chrono::high_resolution_clock::now();
    size_t embedding_count = Enumerate::enumerate(data_graph, query_graph, engine_type.c_str(), table_, candidates_, matching_order_, pivot_, call_count);
    auto end_enumerate_time = std::chrono::high_resolution_clock::now();
    enumerate_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(end_enumerate_time - start_enumerate_time).count();
    std::cout << "Enumerate time: " << NANOSECTOSEC(enumerate_time_) << " s" << std::endl;

#ifdef DEBUGMODE
    std::cout << "Enumerate call count : " << call_count << std::endl;
#endif

    std::cout << "#Embeddings: " << embedding_count << std::endl;

}