#include "CGI_utils.hpp"

// using namespace	std;

// int		cgi(std::string path, const Location &location)
// {
// 	path = location.path + "/" + path;

// 	cout << "path: " << path << endl;
// 	ws::CGI		cgi(path);
// 	return 1;
// }

bool	file_exists(std::string path)
{
	std::ifstream	f(path);
	if (!f.good())
		return false;
	return true;
}

char**	mapToArray(std::map<std::string, std::string> &strings)
{
	char	**array = new char*[strings.size()];
	int		i = 0;

	for (std::map<std::string, std::string>::iterator it = strings.begin(); it != strings.end(); ++i, ++it)
	{
		array[i] = new char[it->second.size() + it->first.size() + 2];
		strcpy(array[i], (it->first + "=" + it->second).c_str());
		std::cout << array[i] << std::endl;
	}
	return array;
}

char**	paths(std::string path)
{
	char	**array = new char*[2];
	array[0] = new char[path.size() + 1];
	strcpy(array[0], path.c_str());
	array[1] = NULL;
	return array;
} 
