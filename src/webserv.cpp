#include "webserv.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        console.err("Usage: ./webserv <config-file>");
        return 1;
    }
    try
    {
        ws::Configuration config(argv[1]);
        config.parse();

#ifdef DEBUG
        config.print();
#endif
        // ws::Server server(config);
        // server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
