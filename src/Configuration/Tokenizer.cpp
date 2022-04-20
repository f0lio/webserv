
#include "Tokenizer.hpp"

namespace parser
{
    Token::Token() {}
    Token::Token(TokenType type, std::string value, size_t line) : _type(type), _value(value), _line(line) {}
    Token::~Token() {}

    Tokenizer::Tokenizer(std::istream &stream) : _stream(stream), _line_number(0) {}
    Tokenizer::~Tokenizer() {}

    std::vector<Token> Tokenizer::getTokens() { return _tokens; };

    void Tokenizer::tokenize()
    {
        char c;
        bool has_identifier = false;
        std::string value;

        _line_number = 1;
        while (_stream.get(c))
        {
            if (c == ' ' || c == '\t')
                continue;
            else if (c == '\n')
                _line_number++;
            else if (c == TOKEN_COMMENT)
            {
                while (_stream.peek() != '\n' && _stream.get(c))
                    value += c;
                _tokens.push_back(Token(COMMENT, value, _line_number));
                value.clear();
                continue;
            }
            else if (c == TOKEN_BLOCK_OPEN || c == TOKEN_BLOCK_CLOSE || c == TOKEN_SDIRECTIVE_CLOSE)
            {
                if (c == TOKEN_BLOCK_OPEN)
                    _tokens.push_back(Token(BLOCK_OPEN, std::string(1, c), _line_number));
                else if (c == TOKEN_BLOCK_CLOSE)
                    _tokens.push_back(Token(BLOCK_CLOSE, std::string(1, c), _line_number));
                else if (c == TOKEN_SDIRECTIVE_CLOSE)
                    _tokens.push_back(Token(SDIRECTIVE_END, std::string(1, c), _line_number));
                has_identifier = false;
            }
            else if (c == '"' || c == '\'')
            {
                char quote = c;

                while (_stream.get(c))
                {
                    if (c == quote)
                    {
                        _tokens.push_back(Token(PARAM_LITERAL, value, _line_number));
                        value.clear();
                        break;
                    }
                    else if (c == '\n')
                        throw std::runtime_error("unexpected newline in string literal");
                    else if (c == '\\')
                    {
                        _stream.get(c);
                        if (c == quote)
                            value += quote;
                        else if (c == '\\')
                            value += '\\';
                        else
                            throw std::runtime_error("Invalid escape sequence");
                    }
                    else
                        value += c;
                }
            }
            else
            {
                std::string value;
                value += c;
                while (_stream.get(c))
                {
                    if (isalnum(c) || c == '_' || c == '.' || c == '/')
                        value += c;
                    else
                    {
                        _stream.unget();
                        if (has_identifier == false)
                        {
                            _tokens.push_back(Token(IDENTIFIER, value, _line_number));
                            has_identifier = true;
                        }
                        else
                            _tokens.push_back(Token(PARAM_LITERAL, value, _line_number));
                        break;
                    }
                }
            }
        }
        _tokens.push_back(Token(END_OF_FILE, "", _line_number));
    }

    bool Tokenizer::hasNext() const
    {
        return _index < _tokens.size();
    }

    Token Tokenizer::next()
    {
        if (_index < _tokens.size())
            return _tokens[_index++];
        return _tokens.back();
    }

    Token Tokenizer::peek() const
    {
        if (_index < _tokens.size())
            return _tokens[_index];
        return _tokens.back();
    }

    void Tokenizer::print() const
    {
        for (std::vector<Token>::const_iterator it = _tokens.begin(); it != _tokens.end(); ++it)
            printf("%3.zu, %-15s=  [%s]\n", it->_line, TokenTypeStrings[it->_type], it->_value.c_str());
    }
}
