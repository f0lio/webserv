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
        config.setup();
        
        std::vector<ws::VServer *> const & vservers = config.getVServers();

        for (size_t i = 0; i < vservers.size(); i++)
        {
            vservers[i]->print();
        }

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
