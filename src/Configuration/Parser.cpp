

#include "Parser.hpp"

namespace parser
{
    Parser::Parser(std::vector<Token> &tokens) : _tokens(tokens)
    {

        std::set<std::string> valid_simple_directives(
            _valid_simple_directives,
            _valid_simple_directives + sizeof(_valid_simple_directives) / sizeof(_valid_simple_directives[0]));

        std::set<std::string> valid_block_directives(
            _valid_block_directives,
            _valid_block_directives + sizeof(_valid_block_directives) / sizeof(_valid_block_directives[0]));
    }
    Parser::~Parser() {}

    void Parser::parse(std::vector<Token> &tokens)
    {
        std::cout << "Parsing.." << std::endl;
    }
    void Parser::print() const
    {
        std::cout << "Printing.." << std::endl;
    }

} // namespace parser