#include "Cluster.hpp"

namespace ws
{
    Cluster::Cluster(Configuration const& config) : _config(config), _running(false), _setup(false)
    {
    }

    Cluster::~Cluster()
    {
    }

    void Cluster::setup()
    {
        std::vector<VServer*> const& servers = _config.getVServers();

        std::vector<VServer*>::const_iterator it = servers.begin();
        for (; it != servers.end(); it++)
        {
            (*it)->start(_binded_listens);
            _fd_to_vserver[(*it)->getFd()] = *it;
        }
        _setup = true;
    }

    void Cluster::run()
    {
        if (_setup == false)
            setup();
        
        console.log("Cluster is running...");

    } // void Cluster::run()

} // namespace ws