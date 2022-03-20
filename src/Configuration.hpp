

#pragma once

#include "webserv.hpp"

std::string valid_keys_[] = {
    "host",
    "port",
    "root",
    "index",
    "name"};

std::vector<std::string> valid_keys;

class Configuration
{
public:
    Configuration(const std::string &path) : _path(path)
    {
        valid_keys.insert(valid_keys.end(), valid_keys_, valid_keys_ + sizeof(valid_keys_) / sizeof(valid_keys_[0]));
    };
    ~Configuration() {}

    void parse()
    {
        std::ifstream file(_path);

        if (!file.is_open())
            throw std::runtime_error("Could not open configuration file");

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::stringstream ss(line);
            std::string key;
            std::string value;

            std::getline(ss, key, '=');
            std::getline(ss, value);

            std::string::iterator it = value.begin();
            while (it != value.end() && isspace(*it))
                ++it;
            value.erase(value.begin(), it);


            if (std::find(valid_keys.begin(), valid_keys.end(), key) == valid_keys.end())
                throw std::runtime_error("Invalid key in configuration file");
            _config[key] = value;
        }
        file.close();
    }

    void print() const
    {
        for (std::map<std::string, std::string>::const_iterator it = _config.begin(); it != _config.end(); ++it)
            std::cout << it->first << " = " << it->second << std::endl;
    }

private:
    std::map<std::string, std::string> _config;
    std::string _path;
};
