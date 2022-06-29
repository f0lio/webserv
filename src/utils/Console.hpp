
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
#ifdef CONSOLE_ON
        std::cerr << "\033[1;31m" << msg << "\033[0m" << std::endl;
#endif
    }

    void warn(const std::string &msg)
    {
#ifdef CONSOLE_ON
        std::cout << "\033[1;33m" << msg << "\033[0m" << std::endl;
#endif
    }

    void log(const std::string &msg)
    {
#ifdef CONSOLE_ON
        std::cout << msg << std::endl;
#endif
    }
    void log(const char *msg)
    {
#ifdef CONSOLE_ON
        std::cout << msg << std::endl;
#endif
    }

// 	template <typename T>
// 	void superlog(T t) 
// 	{
// #ifdef CONSOLE_ON
// 		std::cout << t ;
// #endif
// 	}

// 	template<typename T, typename... Args>
// 	void superlog(T t, Args... args)
// 	{
// #ifdef CONSOLE_ON
// 		std::cout << t ;
// #endif

// 		superlog(args...) ;
// 	}
	
// 	template<typename T, typename... Args>
// 	void log(T t, Args... args)
// 	{
// #ifdef CONSOLE_ON
// 		std::cout << t ;
// #endif

// 		superlog(args...) ;
// 	}
};
