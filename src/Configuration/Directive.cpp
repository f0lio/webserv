

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

    std::string const &SimpleDirective::getKey() const
    {
        return _key;
    }
    std::vector<std::string> const &SimpleDirective::getArgs() const
    {
        return _args;
    }

    // const SimpleDirective &SimpleDirective::operator=(SimpleDirective const &rhs) const
    // {

    // }

    const SimpleDirective &SimpleDirective::operator=(SimpleDirective const &rhs)
    {
        if (this != &rhs)
        {
            _key = rhs._key;
            _args = rhs._args;
        }
        return *this;
    }

    void SimpleDirective::check() const
    {
        // if (_args.size() < DirectiveRules[_key].min_args)
        //     throw std::runtime_error("Directive " + _key + " has too few arguments");
    }

    void SimpleDirective::print() const
    {
        std::cout << _key << "\t:";
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

    std::string const &BlockDirective::getKey() const
    {
        return _key;
    }
    std::vector<std::string> const &BlockDirective::getArgs() const
    {
        return _args;
    }
    // std::map<std::string, SimpleDirective> const &  BlockDirective::getDirectives() const
    // {
    //     return _directives;
    // }
    std::vector<SimpleDirective> const &BlockDirective::getDirectives() const
    {
        return _directives_vec;
    }

    void BlockDirective::check() const
    {
    }

    void BlockDirective::print() const
    {
        std::cout << _key << ":";
        for (auto arg : _args)
        {
            std::cout << " " << arg;
        }
        std::cout << std::endl;
        for (auto smp : _directives_vec)
            smp.print();
    }

} // namespace parser
