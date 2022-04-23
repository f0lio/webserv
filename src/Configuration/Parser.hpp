
#pragma once

#include "Tokenizer.hpp"
#include "Context.hpp"
#include <fstream>
#include <vector>

namespace parser
{
    static const char *_context_identifiers[] = {
        "server"};

    static const char *_simple_identifiers[] = {
        "host",
        "port",
        "root",
        "server_name",
        "error_pages",
        "max_body_size",
        "methods",
    };

    static const char *_block_identifiers[] = {
        "location"};

    static const char *_location_identifiers[] = {
        "methods",
        "root",
        "index",
        "cgi",
    };

    // class Context;
    class Parser
    {
    public:
        Parser(Tokenizer &tokenizer);
        ~Parser();

        void parse();
        void print() const;
        std::vector<Context> getContexts() const;

    private:
        Tokenizer &_tokenizer;
        std::vector<Token> _tokens;
        std::vector<Context> _contexts;

        // parser utils
        void parseContext();
        SimpleDirective parseSimpleDirective(Context &ctx);
        BlockDirective parseBlockDirective(Context &ctx);
        bool isSimpleIdentifier(const std::string &identifier) const;
        bool isBlockIdentifier(const std::string &identifier) const;
        bool isContextIdentifier(const std::string &identifier) const;
        Token _currentToken;
        Token next();
        Token peek() const;
        bool hasNext() const;

        // error formatting
        std::string err_invalid_identifier(const std::string &identifier) const;
        std::string err_unexpected_token(const std::string &dirName) const;
        std::string err_directive_not_open(const std::string &dirName) const;
        std::string err_directive_not_closed(const std::string &dirName) const;
        std::string err_directive_invalid_args(const std::string &dirName) const;
        std::string err_directive_not_terminated(const std::string &dirName) const;
        std::string line_num_msg() const; // returns e.g. "line: 4"
    };

} // namespace parser
