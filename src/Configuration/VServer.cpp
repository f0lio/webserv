#include "VServer.hpp"
#include "Context.hpp"

namespace ws
{
    VServer::VServer(parser::Context const& context)
    {
        std::vector<parser::SimpleDirective> const& dirs = context.getSimpleDirectives();
        std::vector<parser::BlockDirective> const& locs = context.getBlockDirectives();

        for (size_t i = 0; i < dirs.size();i++)
        {
            if (dirs[i].getKey() == "listen")
            {
                struct Listen listen;

                if (dirs[i].getArgs().size() == 1)
                {
                    listen.host = "127.0.0.1";
                    if (is_number(dirs[i].getArgs()[0].c_str()))
                        listen.port = std::atoi(dirs[i].getArgs()[0].c_str());
                    else
                        throw std::runtime_error("Invalid port number: " + dirs[i].getArgs()[0]);
                }
                else if (dirs[i].getArgs().size() == 2)
                {
                    listen.host = dirs[i].getArgs()[0];
                    if (listen.host == "localhost")
                        listen.host = "127.0.0.1";
                    if (is_number(dirs[i].getArgs()[1].c_str()))
                        listen.port = atoi(dirs[i].getArgs()[1].c_str());
                    else
                        throw std::runtime_error("Invalid port number: " + dirs[i].getArgs()[1]);
                }

                _listens.push_back(listen);
            }
            else
                _config[dirs[i].getKey()] = dirs[i].getArgs();
        }

        for (size_t i = 0; i < locs.size(); i++)
        {
            struct Location loc;
            std::vector<parser::SimpleDirective> const& dirs = locs[i].getDirectives();
            for (size_t j = 0; j < dirs.size(); j++)
                loc.config[dirs[j].getKey()] = dirs[j].getArgs();

            loc.path = locs[i].getArgs()[0];

            if (loc.config.find("root") == loc.config.end())
                loc.config["root"] = _config["root"];
            _locations[loc.path] = loc;
        }

    }

    VServer::~VServer()
    {
    }

    t_vec_str const& VServer::get(const std::string& key) const
    {
        if (_config.find(key) != _config.end())
            return _config.find(key)->second;
        else
            return t_vec_str();
    }

    std::map<std::string, struct Location> const& VServer::getLocations() const
    {
        return _locations;
    }

    std::vector<struct Listen> const& VServer::getListens() const
    {
        return _listens;
    }


    void VServer::print() const
    {
        std::cout << "## VServer ##" << std::endl;

        for (size_t i = 0; i < _listens.size(); i++)
        {
            std::cout << "Listen: " << _listens[i].host << ":" << _listens[i].port << std::endl;
        }

        for (auto const& it : _config)
        {
            std::cout << it.first << ": ";
            for (auto const& it2 : it.second)
            {
                std::cout << it2 << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "-> Locations" << std::endl;
        for (auto const& it : _locations)
        {
            std::cout << it.first << ": ";
            for (auto const& it2 : it.second.config)
            {
                std::cout << it2.first << ": ";
                for (auto const& it3 : it2.second)
                {
                    std::cout << it3 << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }


        std::cout << std::endl;
    }
}
