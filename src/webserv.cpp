

#include "webserv.hpp"

typedef unsigned short t_port;

int main(int argc, char *argv[])
{
    if (argc != 2)
        console.err("Usage: ./webserv <config-file>", true);
    try
    {
        Configuration config(argv[1]);

        config.parse();
        // config.print();

        Server server(config);
        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return (0);
}
