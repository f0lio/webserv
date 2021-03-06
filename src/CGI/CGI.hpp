#pragma once

#include "CGI_utils.hpp"

// only one per binary (php, python, ruby, etc.) from which run() is called with the script directory? not sure
class CGI
{
private:
	std::map<std::string, std::string> envp;
	std::string binPath;
	std::string root;
	std::string outputFile;

public:
	CGI(const std::string &binPath, const std::string &root);
	~CGI();
	// const CGI &operator=(const CGI &other);

	int run(ws::Request const &request);
	std::string const & getOutputFile() const;

private:
	CGI();
	int exec(std::string cgiPath, ws::Request const &request);
	void setEnvp(std::string const &cgiPath, ws::Request const &request);
	std::string headerToMetaData(std::string header);

};
