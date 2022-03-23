

#pragma once

#include <map>
#include <string>
#include "./Directive.hpp"

namespace parser
{
    class Directive;
    class Context
    {
    public:
        Context(){};
        ~Context(){};

    private:
        std::string _name;
        std::map<std::string, Directive> _directives;
    };
} // namespace ws