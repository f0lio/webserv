

#pragma once

#include "Tokenizer.hpp"

#include <vector>

namespace parser
{
    static std::string _simple_directives[] = {
        "host",
        "port",
        "root",
        "location",
        "server_names",
        "methods"};

    static std::string _block_directives[] = {
        "location",
        "server"};

    class Parser
    {
    public:
        Parser(Tokenizer &tokenizer);
        ~Parser();

        void parse();
        void print() const;

        bool isValidIdentifier(const std::string &identifier) const;
        bool isValidSimpleDirective(const std::string &directive) const;
        bool isValidBlockDirective(const std::string &directive) const;

    private:
        Tokenizer &_tokenizer;
        std::vector<Token> _tokens;
        std::vector<Token>::iterator current_token;
        std::vector<Token>::iterator end_token;
    };

} // namespace parser
