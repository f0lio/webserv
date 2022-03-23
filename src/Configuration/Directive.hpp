#pragma once

#include "webserv.hpp"

/*

The file consists of a series of sections.

<simple>:  name=param(s)
<block>:   name=param(s) and a block of <simple>s
<context>: name=param(s) and a block of <block>s

example:
    <context name="server">
        <simple name="listen" params="8080">
        <simple name="error_page" params="404 /404.html">
        <block name="location" params="/">
            <simple name="root" value="/"/>
            <simple name="server_names" value="localhost"/>
            <simple name="methods" value="GET POST"/>
        </block>
    </context>

**/

namespace parser
{

    enum DirectiveType
    {
        SIMPLE = 1,
        BLOCK = 2,
        CONTEXT = 4
    };

    // static std::string simple_directives_[] = {
    //     "host",
    //     "port",
    //     "root",
    //     "location",
    //     "server_names",
    //     "methods"};

    // static std::vector<std::string> valid_keys;

    class Directive
    {
    public:
        Directive();
        Directive(const std::string &key, const std::vector<std::string> &_params);
        ~Directive();

        DirectiveType getType();

    private:
        DirectiveType _type;
        std::string _key;
        std::vector<std::string> _params;
        std::map<std::string, Directive> _blocks;
    };

} // namespace parser