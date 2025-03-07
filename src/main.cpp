#include <iostream>
#include <chrono>

#include "command/command.h"

int main(int argc, char** argv) {
    MatchingCommand matching_command(argc, argv);
    std::string input_query_graph_file = matching_command.get_query_graph_file_path();
    std::string input_data_graph_file = matching_command.get_data_graph_file_path();
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

    return 0;
}