#include "command.h"

#include <utility>
#include <algorithm>

CommandParser::CommandParser(int argc, char **argv) {
    for (int i = 1; i < argc; i++)
        tokens_.push_back(std::string(argv[i]));
}

const std::string CommandParser::get_command_option(const std::string &option) const {

    auto it = find(tokens_.begin(), tokens_.end(), option);
    if (it != tokens_.end() && ++it != tokens_.end()) {
        return *it;
    }
    return "";
}

bool CommandParser::command_option_exists(const std::string &option) const {
    return find(tokens_.begin(), tokens_.end(), option) != tokens_.end();
}

MatchingCommand::MatchingCommand(int argc, char **argv) : CommandParser(argc, argv) {
    // Initialize options value
    options_key_[OptionKeyword::QueryGraphFile] = "-q";
    options_key_[OptionKeyword::DataGraphFile] = "-d";
    options_key_[OptionKeyword::Filter] = "-filter";
    options_key_[OptionKeyword::Order] = "-order";
    options_key_[OptionKeyword::Engine] = "-engine";
    options_key_[OptionKeyword::MaxOutputEmbeddingNum] = "-num";
    process_options();
}

void MatchingCommand::process_options() {
    for (auto i = static_cast<std::underlying_type_t<OptionKeyword>>(OptionKeyword::UNDEFINED) + 1; i < static_cast<std::underlying_type_t<OptionKeyword>>(OptionKeyword::MAXKEYWORD); i++) {
        OptionKeyword keyword = (OptionKeyword)i;
        options_value_[keyword] = get_command_option(options_key_[keyword]);
    }
}