#pragma once

#include "Socket.hpp"

namespace ws
{
    class Client : protected Socket
    {
    public:
        Client();
        ~Client();

    private:        
        void prepare();
    };

} // namespace ws
