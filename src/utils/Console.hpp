
#pragma once

#include <iostream>
// #include <stdlib.h>

class Console
{
public:
    Console(){};
    ~Console(){};

    void err(const std::string &msg)
    {
#ifndef CONSOLE_OFF
        std::cerr << "\033[1;31m" << msg << "\033[0m" << std::endl;
#endif
    }

    void warn(const std::string &msg)
    {
#ifndef CONSOLE_OFF
        std::cerr << "\033[1;33m" << msg << "\033[0m" << std::endl;
#endif
    }

    void log(const std::string &msg)
    {
#ifndef CONSOLE_OFF
        std::cout << msg << std::endl;
#endif
    }
};
