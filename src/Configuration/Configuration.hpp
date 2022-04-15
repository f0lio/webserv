#pragma once

#include <map>
#include "./Context.hpp"
#include "./Parser.hpp"


namespace ws
{
    class Configuration
    {
    public:
        Configuration(const std::string &path);
        ~Configuration();

        void parse();
        void print() const;
        std::vector<parser::Context> getContexts() const;

    private:
        std::vector<parser::Context> _servers;
        std::string _path;
    };
}