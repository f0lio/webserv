
#include "Tokenizer.hpp"

namespace parser
{
    Token::Token() {}
    Token::Token(TokenType type, std::string value, size_t line) : _type(type), _value(value), _line(line) {}
    Token::~Token() {}

    Tokenizer::Tokenizer(std::istream &stream) : _stream(stream), _line_number(0) {}
    Tokenizer::~Tokenizer() {}

    std::vector<Token> Tokenizer::getTokens() { return _tokens; };

    void Tokenizer::skipWhitespaces()
    {
        while (_stream.peek() == ' ' || _stream.peek() == '\t')
            _stream.get();
    }

    void Tokenizer::tokenize()
    {
        char c;
        bool has_identifier = false;
        std::string value;
        std::string tSeperators("{};");

        while (_stream.get(c))
        {
            skipWhitespaces();
            if (c == '\n')
            {
                _line_number++;
                has_identifier = false;
            }
            else if (c == '#')
            {
                while (_stream.get(c) && c != '\n')
                    value += c;
                _tokens.push_back(Token(COMMENT, value, _line_number));
                value.clear();
                has_identifier = false;
                continue;
            }
            else if (tSeperators.find(c) != std::string::npos)
            {
                _tokens.push_back(Token(SEPARATOR, std::string(1, c), _line_number));
                has_identifier = false;
            }

            else if (c == '"' || c == '\'')
            {
                char quote = c;

                value = "";
                while (_stream.get(c))
                {
                    if (c == quote)
                    {
                        _tokens.push_back(Token(PARAM_LITERAL, value, _line_number));
                        break;
                    }
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
        {
            std::cout << sTokenTypeStrings[it->_type] << ": " << it->_value << std::endl;
        }
    }

}