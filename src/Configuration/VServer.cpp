#include "VServer.hpp"
#include "Context.hpp"

namespace ws
{
    VServer::VServer(parser::Context const &context)
    {
        std::vector<parser::SimpleDirective> const &dirs = context.getSimpleDirectives();
        std::vector<parser::BlockDirective> const &locs = context.getBlockDirectives();
        
        for (size_t i = 0; i < dirs.size(); i++)
            _config[dirs[i].getKey()] = dirs[i].getArgs();

        for (size_t i = 0; i < locs.size(); i++)
        {
            struct Location loc;
            std::vector<parser::SimpleDirective> const &dirs = locs[i].getDirectives();
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

    t_vec_str const &VServer::get(const std::string &key) const
    {
        return _config.find(key)->second;
    }

    void VServer::print() const
    {
        std::cout << "## VServer ##" << std::endl;
        for (auto const &it : _config)
        {
            std::cout << it.first << ": ";
            for (auto const &it2 : it.second)
            {
                std::cout << it2 << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "-> Locations" << std::endl;
        for (auto const &it : _locations)
        {
            std::cout << it.first << ": ";
            for (auto const &it2 : it.second.config)
            {
                std::cout << it2.first << ": ";
                for (auto const &it3 : it2.second)
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
