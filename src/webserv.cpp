#include "webserv.hpp"

void print_usage()
{
    console.log("usage: ./webserv [config_file]");
}

void handle_flags(int argc, const char* argv[])
{
    if (argc > 3 && strcmp(argv[1], "-t") != 0)
    {
        print_usage();
        exit(2);
    }
    if (argc == 2 && strcmp(argv[1], "-h") == 0)
    {
        print_usage();
        exit(0);
    }
    // checks config file validity without running the server
    else if ((argc == 2 || argc == 3) && strcmp(argv[1], "-t") == 0)
    {
        const char* config_file = argc == 3 ? argv[2] : DEFAULT_CONFIG_FILE;
        try
        {
            ws::Configuration config(config_file);
            config.setup();
        }
        catch (const std::exception& e)
        {
            console.err(e.what());
            exit(1);
        }
        console.log(SERVER_NAME ": the configuration file ",
            config_file, " syntax is ok\n");
        console.log(SERVER_NAME ": the configuration file ",
            config_file, " test is successful\n");
        exit(0);
    }
}

int main(int argc, const char* argv[])
{
    handle_flags(argc, argv);
    try
    {
        ws::Configuration config(argc == 2 ? argv[1] : DEFAULT_CONFIG_FILE);
        config.setup();

        ws::Cluster cluster(config);
        cluster.setup();
        console.log("Loaded [" + config.getPath() + "]");
        cluster.run();
    }
    catch (const std::exception& e)
    {
        console.err(e.what());
        return 1;
    }
    return 0;
}
