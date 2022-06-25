#pragma once

#include "CGI_utils.hpp"

// only one per binary (php, python, ruby, etc.) from which run() is called with the script directory? not sure
class CGI
{
private:
	std::map<std::string, std::string> envp;
	std::string binPath;
	std::string root;

public:
	CGI(const std::string &binPath, const std::string &root);
	~CGI();
	// const CGI &operator=(const CGI &other);

	int run(std::string cgiPath, std::map<std::string, std::string> const &requestEnvp);

private:
	CGI();
	int exec(std::string cgiPath);
	void setEnvp(std::map<std::string, std::string> const &requestEnvp);
	// std::string const &getCGI() const;
};
