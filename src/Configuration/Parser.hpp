
#pragma once

#include "Utils.hpp"
#include "Tokenizer.hpp"
#include "Context.hpp"
#include "ParserRules.hpp"

namespace parser
{
    class SimpleDirective;
    class BlockDirective;
    class Parser
    {
    public:
        Parser(Tokenizer &tokenizer);
        ~Parser();

        void parse();
        void print() const;
        std::vector<Context> const &getContexts() const;

    private:
        Tokenizer &_tokenizer;
        std::vector<Token> _tokens;
        std::vector<Context> _contexts;

        // parser utils
        void parseContext(size_t &contextIndex);
        SimpleDirective parseSimpleDirective(Context &ctx);
        BlockDirective parseBlockDirective(Context &ctx);
        bool isSimpleIdentifier(const std::string &identifier) const;
        bool isBlockIdentifier(const std::string &identifier) const;
        bool isContextIdentifier(const std::string &identifier) const;
        void checkArgs(const std::string &key, const std::vector<std::string> &args) const;

        Token _currentToken;
        Token next();
        Token peek() const;
        bool hasNext() const;

        // error formatting
        std::string err_invalid_identifier(const std::string &identifier) const;
        std::string err_unexpected_token(const std::string &dirName) const;
        std::string err_directive_not_open(const std::string &dirName) const;
        std::string err_directive_not_closed(const std::string &dirName) const;
        std::string err_directive_invalid_args_count(const std::string &dirName) const;
        std::string err_directive_invalid_arg_type(const std::string &dirName) const;
        std::string err_directive_not_terminated(const std::string &dirName) const;
        std::string line_num_msg() const; // returns e.g. "line: 4"
    };

} // namespace parser
