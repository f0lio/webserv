
#include "CGI.hpp"

CGI::CGI(const std::string &binPath, const std::string &root) : binPath(binPath), root(root)
{
}

CGI::CGI() : binPath()
{
}

// const CGI &CGI::operator=(const CGI &other)
// {
// 	this->binPath = other.binPath;
// 	// this->name = other.name;
// 	return *this;
// }

CGI::~CGI()
{
}

int CGI::run(ws::Request const &request) // https://www.rfc-editor.org/rfc/rfc3875.html#section-4
{
	if (!is_regular_file(binPath))
		return 404;

	if (!is_executable_file(binPath))
		return 403;

	std::string cgiPath = request.getPath();

	std::cout << "cgiPath: " << cgiPath << std::endl;

	if (cgiPath.find("/", 1) != std::string::npos)
		cgiPath = cgiPath.substr(0, cgiPath.find("/", 1));

	std::cout << "cgiPath: " << cgiPath << std::endl;

	if (cgiPath.find("..") != std::string::npos)
		return 403; // bad boy hacker

	cgiPath = root + "/" + cgiPath;

	if (!is_regular_file(cgiPath))
		return 404;

	if (!is_readable_file(cgiPath))
		return 403;

	setEnvp(cgiPath, request);

	exec(cgiPath, request);
}

std::string CGI::headerToMetaData(std::string header)
{
	for (size_t i = 0; i < header.size(); i++)
	{
		if (header[i] == '-')
			header[i] = '_';
	}
	header.insert(0, "HTTP_");

	return header;
}

void CGI::setEnvp(std::string const &cgiPath, ws::Request const &request)
{
	std::map<std::string, std::string> const &reqenvp = request.getHeaders();

	for (std::map<std::string, std::string>::const_iterator it = reqenvp.begin(); it != reqenvp.end(); ++it)
		envp[headerToMetaData(it->first)] = it->second;

	envp["AUTH_TYPE"] = "";
	envp["CONTENT_LENGTH"] = envp["HTTP_CONTENT_LENGTH"];
	envp["SERVER_SOFTWARE"] = "webserv";

	if (envp["HTTP_HOST"].find(":") != std::string::npos)
	{
		envp["SERVER_NAME"] = envp["HTTP_HOST"].substr(0, envp["HTTP_HOST"].find(":"));
		envp["PORT"] = envp["HTTP_HOST"].substr(envp["HTTP_HOST"].find(":"));
	}
	else
	{
		envp["SERVER_NAME"] = envp["HTTP_HOST"];
		envp["PORT"] = "";
	}

	envp["GATEWAY_INTERFACE"] = "CGI/1.1";
	envp["SERVER_PROTOCOL"] = "HTTP/1.1";

	std::cout << "request.getPath(): " << request.getPath() << std::endl;

	if (request.getPath() != cgiPath)
	{
		envp["PATH_INFO"] = request.getPath().substr(cgiPath.size());
		envp["PATH_TRANSLATED"] = root + envp["PATH_INFO"];
	}
	else
	{
		envp["PATH_INFO"] = "";
		envp["PATH_TRANSLATED"] = root + envp["PATH_INFO"];
	}

	std::cout << "envp[\"PATH_INFO\"]: " << envp["PATH_INFO"] << std::endl;

	envp["QUERY_STRING"] = request.getQuery();
	envp["REMOTE_HOST"] = "";
	envp["REQUEST_METHOD"] = request.getMethod();
	envp["SCRIPT_NAME"] = cgiPath;
}

int CGI::exec(std::string cgiPath, ws::Request const &request)
{
	pid_t pid = fork();

	char tempOFile[] = "/tmp/cgiOutFile_XXXXXX";

	int fdo = mkstemp(tempOFile);

	cgiOutFile.assign(tempOFile);

	std::cout << "tempOFile: " << tempOFile << std::endl;

	if (fdo == -1)
		return 500; // internal server error



	char tempIfile[] = "/tmp/cgiInFile_XXXXXX";

	int fdi = mkstemp(tempIfile);

	if (fdi == -1)
	{
		close(fdo);
		return 500; // internal server error
	}


	if (!pid)
	{
	std::ofstream ofile(tempOFile);

	ofile << request.getBody();

		dup2(fdo, STDOUT_FILENO);
		dup2(fdi, STDIN_FILENO);

		int ret = execle(binPath.c_str(), binPath.c_str(), cgiPath.c_str(), NULL, mapToArray(envp));
		exit(1); // execve failed
	}

	close(fdo);
	close(fdi);

	if (pid > 0)
		return 1;

	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return WEXITSTATUS(status);

	return 0;
}
