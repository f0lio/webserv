
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
            {
                _line_number++;
                has_identifier = false;
            }
            else if (c == '#')
            {
                while (_stream.peek() != '\n' && _stream.get(c))
                    value += c;
                _tokens.push_back(Token(COMMENT, value, _line_number));
                value.clear();
                has_identifier = false;
                continue;
            }
            else if (c == '{' || c == '}' || c == ';')
            {
                _tokens.push_back(Token(SEPARATOR, std::string(1, c), _line_number));
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

    void Tokenizer::print()
    {
        for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
            printf("%3.zu, %-15s=  [%s]\n", it->_line, sTokenTypeStrings[it->_type], it->_value.c_str());
    }

    bool Tokenizer::hasNext()
    {
        return _tokens.size() > 0;
    }

    Token Tokenizer::next()
    {
        if (_tokens.size() > 0)
        {
            _current = _tokens.front();
            _tokens.erase(_tokens.begin());
            return _current;
        }
        else
            throw std::runtime_error("No more tokens");
    }
}
