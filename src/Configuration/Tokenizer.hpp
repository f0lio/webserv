
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <set>

namespace parser
{

    enum TokenType
    {
        WHITESPACE,
        IDENTIFIER,
        INTEGER_LITERAL,
        STRING_LITERAL,
        POTENTIAL_COMMENT,
        COMMENT
    };

    class Token
    {
    public:
        Token() : type(WHITESPACE), value("") {}
        Token(TokenType type, std::string value) : type(type), value(value) {}
        ~Token() {}

    private:
        TokenType type;
        std::string value;
        size_t line;
    };

    class Tokenizer
    {
    public:
        Tokenizer(std::istream &stream);
        ~Tokenizer();
        Token next();
        Token peek();
        void skipWhitespace();
        void skipComment();

    private:
        std::istream &_stream;
        Token _current;
        Token _next;
        bool _hasNext;
    };

}
