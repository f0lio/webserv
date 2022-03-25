

#pragma once

#include "Tokenizer.hpp"

#include <vector>

namespace parser
{

    static std::string _valid_simple_directives[] = {
        "host",
        "port",
        "root",
        "location",
        "server_names",
        "methods"};

    static std::string _valid_block_directives[] = {
        "location"};

    class Parser
    {
    public:
        Parser(std::vector<Token> &tokens);
        ~Parser();

        void parse(std::vector<Token> &tokens);
        void print() const;

    private:
        std::vector<Token>::iterator current_token;
        std::vector<Token>::iterator end_token;
        std::vector<Token> _tokens;
    };

} // namespace parser
