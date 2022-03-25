
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>

namespace parser
{
    enum TokenType
    {
        SEPARATOR,
        IDENTIFIER,
        PARAM_LITERAL, // STRING | NUMBER
        COMMENT,
        END_OF_FILE
    };

    static const char *sTokenTypeStrings[] = {

        "SEPARATOR",
        "IDENTIFIER",
        "PARAM_LITERAL",
        "COMMENT",
        "END_OF_FILE"};

    // array of valid identifiers
    static const char *validIdentifiers[] = {
        "listen",
        "host", // tempo
        "port",
        "root",
        "location",
        "index",
        "server_names",
        "methods"};

    class Token
    {
    public:
        Token();
        Token(TokenType type, std::string value, size_t line);
        ~Token();

        friend class Tokenizer;

    private:
        TokenType _type;
        std::string _value;
        size_t _line;
    };

    class Tokenizer
    {
    public:
        Tokenizer(std::istream &stream);
        ~Tokenizer();

        void tokenize();
        void print();

        std::vector<Token> getTokens();

    private:
        std::vector<Token> _tokens;
        std::istream &_stream;
        Token _current;
        Token _next;
        size_t _line_number;
        bool _hasNext;
    };
}
