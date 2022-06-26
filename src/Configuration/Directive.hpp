#pragma once

#include "Context.hpp"

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
    // class Context;
    class SimpleDirective
    {
        friend class Parser;

    public:
        SimpleDirective();
        SimpleDirective(const std::string& key, const std::vector<std::string>& params);
        ~SimpleDirective();

        const SimpleDirective& operator=(SimpleDirective const& rhs);

        std::string const& getKey() const;
        std::vector<std::string> const& getArgs() const;
        void check() const;
        void check_occurrence(
            std::map<std::string, int>& loaded_directives,
            std::string const& key) const;
        void print() const;

    private:
        std::string _key;
        std::vector<std::string> _args;
    };

    class BlockDirective
    {
        friend class Parser;

    public:
        BlockDirective();
        BlockDirective(const std::string& key, const std::vector<std::string>& _params);
        ~BlockDirective();

        std::string const& getKey() const;
        std::vector<std::string> const& getArgs() const;

        void setKey(const std::string& key);
        
        // std::map<std::string, SimpleDirective> const & getDirectives() const;
        std::vector<SimpleDirective> const& getDirectives() const;

        void addDirective(const SimpleDirective& dir)
        {
            // std::map<std::string, SimpleDirective> _directives;
            _directives_vec.push_back(dir);
        }
        void check() const;
        void print() const;

    private:
        std::string _key;
        std::vector<std::string> _args;
        std::map<std::string, SimpleDirective> _directives;
        std::vector<SimpleDirective> _directives_vec;
    };

} // namespace parser
