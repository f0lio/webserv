#include "Configuration.hpp"

namespace ws
{
    Configuration::Configuration(const std::string &path) : _path(path)
    {
    }

    Configuration::~Configuration()
    {
        for (size_t i = 0; i < _vservers.size(); i++)
        {
            delete _vservers[i];
        }
    }

    void Configuration::parse()
    {
        std::ifstream file(_path.c_str());
        if (!file.is_open())
            throw std::runtime_error("Can't open file: " + _path);

        parser::Tokenizer tokenizer(file);
        parser::Parser parser(tokenizer);

        parser.parse();
        _contexts = parser.getContexts();
        file.close();
    }

    void Configuration::prepare()
    {
        for (size_t i = 0; i < _contexts.size(); i++)
        {
            parser::Context &ctx = _contexts[i];
            ctx.prepare();
        }
    }

    void Configuration::setup()
    {
        bool default_is_set = false; // to avoid running find() more than once

        this->parse();
        this->prepare();
        for (size_t i = 0; i < _contexts.size(); i++)
        {
            VServer *vserver = new VServer(_contexts[i]);
            _vservers.push_back(vserver);
        }

        for (size_t i = 0; i < _vservers.size(); i++)
        {
            std::vector<std::string> const &serverNames = _vservers[i]->get("server_name");
            port_t port = std::stoi(_vservers[i]->get("listen")[0]);
            for (size_t j = 0; j < serverNames.size(); j++)
            {
                if (_serverNamesMap[port].vservers.find(serverNames[j]) != _serverNamesMap[port].vservers.end())
                    throw std::runtime_error("Duplicate server_name: " + serverNames[j]);
                _serverNamesMap[port].vservers[serverNames[j]] = _vservers[i];
                if (default_is_set == false && _serverNamesMap[port].vservers.find(DEFAULT_SERVER_KEY) == _serverNamesMap[port].vservers.end())
                {
                    default_is_set = true;
                    _serverNamesMap[port].vservers[DEFAULT_SERVER_KEY] = _vservers[i];
                }
            }
        }

        for (size_t i = 0; i < _vservers.size(); i++)
        {
            _vservers[i]->print();
        }
    }

    void Configuration::print() const
    {
        std::cout << "Printing.." << std::endl;
    }

    std::vector<VServer *> const &Configuration::getVServers() const
    {
        return _vservers;
    }

    std::map<port_t, struct ServerName> const &Configuration::getServerNamesMap() const
    {
        return _serverNamesMap;
    }

} // namespace ws
