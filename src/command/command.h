#pragma once

#include <string>
#include <map>
#include <vector>

enum class OptionKeyword {
    UNDEFINED,         // Begin of keyword enum
    QueryGraphFile,    // -q, The query graph file path, compulsive parameter
    DataGraphFile,      // -d, The data graph file path, compulsive parameter
    Filter,             // -filter, The strategy of filtering
    Order,              // -order, The strategy of ordering
    Engine,             // -engine, The computation engine
    MaxOutputEmbeddingNum, // -num, The maximum output embedding num
    MAXKEYWORD, // End of keyword enum
};


class CommandParser {
public:
    CommandParser(int argc, char **argv);

    const std::string get_command_option(const std::string &option) const;
    bool command_option_exists(const std::string &option) const;

private:
    std::vector<std::string> tokens_;
};

class MatchingCommand : public CommandParser {
public:
    MatchingCommand(int argc, char **argv);

    std::string get_query_graph_file_path() {
        return options_value_[OptionKeyword::QueryGraphFile];
    }

    std::string get_data_graph_file_path() {
        return options_value_[OptionKeyword::DataGraphFile];
    }

    std::string get_filter_type() {
        return options_value_[OptionKeyword::Filter] == "" ? "DPiso" : options_value_[OptionKeyword::Filter];
    }

    std::string get_order_type() {
        return options_value_[OptionKeyword::Order] == "" ? "GQL" : options_value_[OptionKeyword::Order];
    }

    std::string get_engine_type() {
        return options_value_[OptionKeyword::Engine] == "" ? "LFTJ" : options_value_[OptionKeyword::Engine];
    }

    unsigned int get_maximum_embedding_num() {
        return options_value_[OptionKeyword::MaxOutputEmbeddingNum] == "" ? 10000 : std::stoi(options_value_[OptionKeyword::MaxOutputEmbeddingNum]);
    }

private:
    void process_options();

private:
    std::map<OptionKeyword, std::string> options_key_;
    std::map<OptionKeyword, std::string> options_value_;
};