
#include "webserv.hpp"

bool  is_included(char c, char *str)
{
    while (str)
        if (c == *str++)
            return true;
    return false;
}
