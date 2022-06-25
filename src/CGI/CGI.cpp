
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

	if (!is_regular_file(cgiPath))
		return 404; // file not found

	if (!is_executable_file(cgiPath))
		return 403; // wrong permissions

	setEnvp(requestEnvp);

	exec(cgiPath);
}

void CGI::setEnvp(std::map<std::string, std::string> const &requestEnvp)
{
	this->envp = requestEnvp;

	// envp["PATH"] = binPath;
	// envp["SERVER_SOFTWARE"] = "webserv";
	// envp["SERVER_NAME"] = SERVER_NAME;
	
}

std::string CGI::getDate()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%d_%m_%Y_%H_%M_%S", timeinfo);
	return buffer;
}

int CGI::exec(std::string cgiPath)
{
	pid_t pid = fork();

	if (pid > 0)
		return 1;
	if (!pid)
	{
		char *tempFile = "/tmp/cgi_temp_file_XXXXXX";
		int fd = mkstemp(tempFile);

		if (fd == -1)
			return 500; // internal server error

		std::string const tempFilePath(tempFile);

		

		int ret = execle(binPath.c_str(), binPath.c_str(), cgiPath.c_str(), NULL, mapToArray(envp));
		exit(1); // execve failed
	}
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return WEXITSTATUS(status);
	return 0;
}
