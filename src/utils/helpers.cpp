
#include "webserv.hpp"

bool  is_included(char c, char *str)
{
    while (str)
        if (c == *str++)
            return true;
    return false;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_number(const char *s)
{
    while (*s)
        if (!std::isdigit(*s++))
            return false;
    return true;
}
