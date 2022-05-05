#include "Configuration.hpp"

namespace ws
{
    Configuration::Configuration(const std::string &path) : _path(path)
    {
    }

    Configuration::~Configuration()
    {
        for (size_t i = 0; i < _vservers.size(); i++)
            delete _vservers[i];
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
        std::cout << _contexts.size() << " contexts" << std::endl;
        for (size_t i = 0; i < _contexts.size(); i++)
            _vservers.push_back(new VServer(_contexts[i]));

        for (size_t i = 0; i < _vservers.size(); i++)
        {       
            std::vector<std::string> const &serverNames = _vservers[i]->get("server_name");

            port_t port;
            std::string host;
            

            // std::vector<Listen> const &listens = _vservers[i]->getListens();

            // for (size_t j = 0; j < listens.size(); j++)
            // {
            //     port = listens[j].port;
            //     host = listens[j].host;
            // }

            for (size_t j = 0; j < serverNames.size(); j++)
            {
                if (this->_serverNamesMap[port].vservers.find(serverNames[j]) != this->_serverNamesMap[port].vservers.end())
                    throw std::runtime_error("Duplicate server_name: " + serverNames[j]);
                this->_serverNamesMap[port].vservers[serverNames[j]] = _vservers[i];
                if (
                    default_is_set == false &&
                    this->_serverNamesMap[port].vservers.find(DEFAULT_SERVER_KEY) == this->_serverNamesMap[port].vservers.end()
                )
                {
                    default_is_set = true;
                    this->_serverNamesMap[port].vservers[DEFAULT_SERVER_KEY] = _vservers[i];
                }
            }
        }

        console.log("Configuration loaded");
        std::cout << _vservers.size() << " vservers loaded" << std::endl;
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
        return this->_serverNamesMap;
    }

} // namespace ws
