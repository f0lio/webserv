

#include "./Directive.hpp"

namespace parser
{
    SimpleDirective::SimpleDirective(
        const std::string &key, const std::vector<std::string> &params)
        : _key(key), _args(params)
    {
        ;
    }

    SimpleDirective::SimpleDirective()
    {
    }

    SimpleDirective::~SimpleDirective()
    {
    }

    std::string SimpleDirective::getKey() const
    {
        return _key;
    }
    std::vector<std::string> SimpleDirective::getArgs() const
    {
        return _args;
    }

    const SimpleDirective &SimpleDirective::operator=(SimpleDirective const &rhs)
    {
        if (this != &rhs)
        {
            _key = rhs._key;
            _args = rhs._args;
        }
        return *this;
    }

    void SimpleDirective::print() const
    {
        std::cout << _key;
        for (auto arg : _args)
        {
            std::cout << " " << arg;
        }
        std::cout << std::endl;
    }

    ///////////////////

    BlockDirective::BlockDirective()
    {
    }

    BlockDirective::BlockDirective(const std::string &key, const std::vector<std::string> &_params)
    {
    }

    BlockDirective::~BlockDirective()
    {
    }

    std::string BlockDirective::getKey() const
    {
        return _key;
    }
    std::string BlockDirective::getArgs() const
    {
        return _arg;
    }
    std::map<std::string, SimpleDirective> BlockDirective::getDirectives() const
    {
        return _directives;
    }
    void BlockDirective::print() const
    {
        std::cout << _key << " " << _arg << std::endl;
        for (auto smp : _directives_vec)
            smp.print();
    }

} // namespace parser
