#include "CGI_utils.hpp"

// using namespace	std;

// int		cgi(std::string path, const Location &location)
// {
// 	path = location.path + "/" + path;

// 	cout << "path: " << path << endl;
// 	ws::CGI		cgi(path);
// 	return 1;
// }

void	mapToArray(std::map<std::string, std::string> &strings)
{
	// char	**array = new char*[strings.size()];
	int		i = 0;

	for (std::map<std::string, std::string>::iterator it = strings.begin(); it != strings.end(); ++i, ++it)
		setenv(it->first.c_str(), it->second.c_str(), 1);
	// return array;
}

char**	paths(std::string path, std::string script)
{
	char	**array = new char*[3];
	array[0] = new char[path.size() + 1];
	array[1] = new char[script.size() + 1];
	strcpy(array[0], path.c_str());
	
	strcpy(array[1], script.c_str());
	
	array[2] = NULL;
	return array;
} 
