#include <iostream>
#include <chrono>

#include "command/command.h"
#include "graph/graph.h"

int main(int argc, char** argv) {
    MatchingCommand matching_command(argc, argv);
    std::string input_data_graph_file = matching_command.get_data_graph_file_path();
    std::string input_query_graph_file = matching_command.get_query_graph_file_path();
    std::string filter_type = matching_command.get_filter_type();
    std::string order_type = matching_command.get_order_type();
    std::string engine_type = matching_command.get_engine_type();
    std::string max_output_embedding_num = matching_command.get_maximum_embedding_num();

    /**
     * Output the command line information.
     */
    std::cout << "Command Line:" << std::endl;
    std::cout << "\tData Graph: " << input_data_graph_file << std::endl;
    std::cout << "\tQuery Graph: " << input_query_graph_file << std::endl;
    std::cout << "\tFilter Type: " << filter_type << std::endl;
    std::cout << "\tOrder Type: " << order_type << std::endl;
    std::cout << "\tEngine Type: " << engine_type << std::endl;
    std::cout << "\tOutput Limit: " << max_output_embedding_num << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    /**
     * Load input graphs.
     */
    std::cout << "2. Load graphs" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Graph data_graph;
    data_graph.load_graph_from_file(input_data_graph_file);

    Graph query_graph;
    query_graph.load_graph_from_file(input_query_graph_file);

    auto end = std::chrono::high_resolution_clock::now();
    double load_graphs_time_in_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << "Data Graph Meta Information" << std::endl;
    data_graph.print_graph_meta_data();

    std::cout << "Query Graph Meta Information" << std::endl;
    query_graph.print_graph_meta_data();
    std::cout << "--------------------------------------------------------------------" << std::endl;

    return 0;
}