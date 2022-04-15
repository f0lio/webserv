
#include "Parser.hpp"

#include "Utils.hpp"

namespace parser
{
    Parser::Parser(Tokenizer &tokenizer) : _tokenizer(tokenizer)
    {
        _tokenizer.tokenize();
    }

    Parser::~Parser() {}

    void Parser::parse()
    {
        while (hasNext() && peek()._type != END_OF_FILE)
        {
            next();
            if (_currentToken._type == COMMENT)
                continue;
            else if (_currentToken._type == IDENTIFIER)
            {
                if (isContextIdentifier(_currentToken._value))
                    parseContext();
                else
                    throw std::runtime_error("invalid identifier");
            }
            else
                throw std::runtime_error("invalid token");
        }
    }

    void Parser::parseContext()
    {
        Context ctx;

        if (peek()._type != BLOCK_OPEN)
            throw std::runtime_error(
                err_directive_not_open(_currentToken._value));

        ctx.setName(_currentToken._value);
        next();

        while (hasNext() && peek()._type != END_OF_FILE)
        {
            next();
            if (_currentToken._type == COMMENT)
                continue;
            else if (isSimpleIdentifier(_currentToken._value))
                ctx.addDirective(parseSimpleDirective(ctx));
            else if (isBlockIdentifier(_currentToken._value))
                ctx.addDirective(parseBlockDirective(ctx));
            else if (_currentToken._type == BLOCK_CLOSE)
                break;
            else
                throw std::runtime_error("invalid identifier");
        }

        if (_currentToken._type != BLOCK_CLOSE)
            throw std::runtime_error(
                err_directive_not_closed(ctx.getName()));
        _contexts.push_back(ctx);
    }

    SimpleDirective Parser::parseSimpleDirective(Context &ctx)
    {
        SimpleDirective dir;

        dir._key = _currentToken._value;
        while (hasNext() && peek()._type != END_OF_FILE)
        {
            next();
            if (_currentToken._type == COMMENT)
                continue;
            else if (_currentToken._type == SDIRECTIVE_END)
                break;
            else if (_currentToken._type == PARAM_LITERAL)

                dir._args.push_back(_currentToken._value);

            else
            {
                throw std::runtime_error(
                    err_directive_not_closed(ctx.getName()));
            }
        }

        if (_currentToken._type != SDIRECTIVE_END)
            throw std::runtime_error(
                err_directive_not_terminated(dir._key));
        else if (dir._args.size() == 0)
        {
            throw std::runtime_error(
                err_directive_invalid_args(ctx.getName())); // hadi
        }
        return dir;
    }

    // only location directives, and no nested blocks
    BlockDirective Parser::parseBlockDirective(Context &ctx)
    {
        BlockDirective dir;

        dir._key = _currentToken._value;

        while (hasNext() && peek()._type != END_OF_FILE)
        {
            next();
            if (_currentToken._type == COMMENT)
                continue;
            else if (_currentToken._type == PARAM_LITERAL)
            {
                if (dir._arg.size())
                    err_directive_invalid_args(dir._key);
                dir._arg = _currentToken._value;
            }
            else if (_currentToken._type == BLOCK_OPEN)
                break;
            else
                err_unexpected_token(dir._key);
        }
        while (hasNext() && peek()._type != END_OF_FILE)
        {
            next();
            if (_currentToken._type == COMMENT)
                continue;
            else if (_currentToken._type == BLOCK_CLOSE)
                break;

            SimpleDirective smp;
            smp = parseSimpleDirective(ctx);
            dir.addDirective(smp);
        }
        if (_currentToken._type != BLOCK_CLOSE)
            err_directive_not_closed(dir._key);

        return dir;
    }

    bool Parser::isSimpleIdentifier(const std::string &identifier) const
    {
        if (identifier.empty())
            return false;

        for (unsigned short i = 0; i < ARRAY_SIZE(_simple_identifiers); i++)
        {
            if (identifier == _simple_identifiers[i])
                return true;
        }
        return false;
    }

    bool Parser::isBlockIdentifier(const std::string &identifier) const
    {
        if (identifier.empty())
            return false;
        for (unsigned short i = 0; i < ARRAY_SIZE(_block_identifiers); i++)
        {
            if (identifier == _block_identifiers[i])
                return true;
        }
        return false;
    }

    bool Parser::isContextIdentifier(const std::string &identifier) const
    {
        if (identifier.empty())
            return false;

        for (unsigned short i = 0; i < ARRAY_SIZE(_context_identifiers); i++)
        {
            if (identifier == _context_identifiers[i])
                return true;
        }
        return false;
    }

    bool Parser::hasNext() const
    {
        return _tokenizer.hasNext();
    }

    Token Parser::next()
    {
        _currentToken = _tokenizer.peek();
        return _tokenizer.next();
    }

    Token Parser::peek() const
    {
        return _tokenizer.peek();
    }

    std::string Parser::line_num_msg() const
    {
        return std::string("line:"); //+ std::string(strtod());
    }

    std::string Parser::err_directive_not_open(const std::string &dirName) const
    {
        std::stringstream ss;
        ss << "line:" << _currentToken._line << ", "
           << "directive \"" + dirName + "\" has no opening \""
           << "}"
           << "\"";
        return ss.str();
    }

    std::string Parser::err_directive_not_closed(const std::string &dirName) const
    {
        std::stringstream ss;
        ss << "line:" << _currentToken._line << ", "
           << "directive \"" + dirName + "\" has no closing \""
           << "}"
           << "\"";
        return ss.str();
    }

    std::string Parser::err_directive_invalid_args(const std::string &dirName) const
    {
        std::stringstream ss;
        ss << "line:" << _currentToken._line << ", "
           << "invalid number of arguments in \"" + dirName + "\" directive";
        return ss.str();
    }

    std::string Parser::err_unexpected_token(const std::string &dirName) const
    {
        std::stringstream ss;
        ss << "line:" << _currentToken._line << ", "
           << "unexpected token after \"" + dirName + "\" directive";
        return ss.str();
    }

    std::string Parser::err_directive_not_terminated(const std::string &dirName) const
    {
        std::stringstream ss;
        ss << "line:" << _currentToken._line << ", "
           << "directive \"" + dirName + "\" is not terminated by \""
           << ";"
           << "\"";
        return ss.str();
    }

    void Parser::print() const
    {
        console.log("# Parser - Printing");
        for (auto ctx : _contexts)
        {
            console.log("## Simple Directives ##");
            for (auto smp : ctx.getSimpleDirectives())
                smp.print();
            console.log("\n## Block Directives ##");
            for (auto dir : ctx.getBlockDirectives())
                dir.print();
        }
    }
} // namespace parser
