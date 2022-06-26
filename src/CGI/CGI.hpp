#pragma once

#include "CGI_utils.hpp"

// only one per binary (php, python, ruby, etc.) from which run() is called with the script directory? not sure
class CGI
{
private:
	std::map<std::string, std::string> envp;
	std::string binPath;
	// std::string							name;

public:
	CGI(const std::string binPath) : binPath(binPath) {}
	CGI() : binPath() {}
	~CGI() {}

	int run(std::string cgiPath)
	{
		if (!file_exists(cgiPath))
			return 1; // file not found?

		// setup the environment variables

		return this->exec(cgiPath);
	}

	const CGI &operator=(const CGI &other)
	{
		this->binPath = other.binPath;
		// this->name = other.name;
		return *this;
	}

private:
	int exec(std::string cgiPath)
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
	// std::string const &getCGI() const;
};