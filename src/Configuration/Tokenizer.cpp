

#include "Tokenizer.hpp"

namespace parser
{

    Tokenizer::Tokenizer(std::istream &stream) : _stream(stream), _hasNext(false) {}

    Tokenizer::~Tokenizer() {}

    Token Tokenizer::next()
    {
        if (_hasNext)
        {
            _current = _next;
            _hasNext = false;
        }
        return _current;
    }

    Token Tokenizer::peek()
    {
        if (!_hasNext)
        {
            _next = next();
            _hasNext = true;
        }
        return _next;
    }

    void Tokenizer::skipWhitespace()
    {
        // while (peek().type == WHITESPACE) {
        //     next();
        // }
    }

    void Tokenizer::skipComment()
    {
        // if (peek().type == POTENTIAL_COMMENT) {
        //     next();
        //     while (peek().type != COMMENT) {
        //         next();
        //     }
        // }
    }

}