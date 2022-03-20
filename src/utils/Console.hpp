
#include "webserv.hpp"

class Console
{
public:
    Console() {};
    ~Console() {};

    void err(const std::string& msg, bool exit_ = false)
    {
        std::cerr << "\033[1;31m" << msg << "\033[0m" << std::endl;
        if (exit_)
            exit(1);
    }

    void warn(const std::string& msg)
    {
        std::cerr << "\043[1;33m" << msg << "\033[0m" << std::endl;
    }

    void log(const std::string& msg)
    {
        std::cout << msg << std::endl;
    }
}  console;
