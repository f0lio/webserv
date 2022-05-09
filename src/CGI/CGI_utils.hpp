#pragma once

#include "CGI.hpp"

struct	Location;

// int		cgi(std::string path, const Location &location);

bool	file_exists(std::string);

char**	map_to_envp(std::map<std::string, std::string> &strings);

char**	paths(std::string path);