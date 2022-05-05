#pragma once

#include "webserv.hpp"

namespace ws
{

    class Cluster
    {
    public:
        Cluster(Configuration const &config);
        ~Cluster();

        void setup();
        void run();

    private:
        Configuration const &_config;
    };

} // namespace ws