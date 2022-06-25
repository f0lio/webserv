
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

int CGI::run(std::string cgiPath, std::map<std::string, std::string> const &requestEnvp)
{
	cgiPath = root + "/" + cgiPath;

	if (!is_readable_file(cgiPath))
		return 404; // file not found?

	setEnvp(requestEnvp);

	return this->exec(cgiPath);
}

void CGI::setEnvp(std::map<std::string, std::string> const &requestEnvp)
{
	this->envp = requestEnvp;

	// envp["PATH"] = binPath;
	// envp["SERVER_SOFTWARE"] = "webserv";
	// envp["SERVER_NAME"] = SERVER_NAME;
	
}

int CGI::exec(std::string cgiPath)
{
	pid_t pid = fork();

	if (pid > 0)
		return 1;
	if (!pid)
	{
		int ret = execle(binPath.c_str(), binPath.c_str(), cgiPath.c_str(), NULL, mapToArray(envp));
		exit(1); // execve failed
	}
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	return 0;
}
