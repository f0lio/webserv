
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "./Utils.hpp"
#include "../src/utils/Console.hpp"
#include "../src/utils/Logger.hpp"
#include "../src/Configuration/Configuration.hpp"
#include "../src/Configuration/Directive.hpp"
#include "../src/Configuration/Context.hpp"
#include "../src/Networking/Server.hpp"

typedef unsigned short port_t;

bool  is_included(char c, char *str);

static Console console;