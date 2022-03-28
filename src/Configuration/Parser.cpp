

#include "Parser.hpp"

namespace parser
{
    Parser::Parser(Tokenizer &tokenizer) : _tokenizer(tokenizer)
    {
    }

    Parser::~Parser() {}

    void Parser::parse()
    {
        _tokenizer.tokenize();
        _tokens = _tokenizer.getTokens();
        _tokenizer.print();
    }

    void Parser::print() const
    {
    }

    bool Parser::isValidIdentifier(const std::string &identifier) const
    {
        if (identifier.empty())
            return false;

        for (size_t i = 0; i < (sizeof(_simple_directives) / sizeof(_simple_directives[0])); i++)
        {
            if (identifier == _simple_directives[i])
                return true;
        }

        for (size_t i = 0; i < (sizeof(_block_directives) / sizeof(_block_directives[0])); i++)
        {
            if (identifier == _block_directives[i])
                return true;
        }
        return false;
    }

} // namespace parser
