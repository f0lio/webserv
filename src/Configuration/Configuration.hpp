

#pragma once

#include <map>
#include "./Context.hpp"

#define SSTR(x) static_cast<std::ostringstream &>(           \
                    (std::ostringstream() << std::dec << x)) \
                    .str()
namespace ws
{
    class Context;
    class Configuration
    {
    public:
        Configuration(const std::string &path);
        ~Configuration();

        void parse();
        void print() const;

    private:
        std::map<int, parser::Context> _servers;
        std::string _path;
    };
}