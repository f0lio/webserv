#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h> //printf
#include <netdb.h>

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "../src/utils/Console.hpp"
#include "../src/utils/Logger.hpp"

#define ARRAY_SIZE(X_X) (sizeof(X_X) / sizeof(X_X[0]))

#define SSTR( x )       static_cast< std::ostringstream & >( \
                        ( std::ostringstream() << std::dec << x  ) ).str()


#define PRINT_TOKEN(TOKEN) printf("%3.zu, %-15s=  [%s]\n",\
                                TOKEN._line,\
                                TokenTypeStrings[TOKEN._type],\
                                TOKEN._value.c_str());

#define PASS 0