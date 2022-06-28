
#include "CGI.hpp"

CGI::CGI(const std::string& binPath, const std::string& root) : binPath(binPath), root(root)
{
}

CGI::CGI() : binPath()
{
}

CGI::~CGI()
{
}
// std::string const & CGI::getOutputFile() const

std::string const& CGI::getOutputFile() const
{
	return outputFile;
}

int CGI::run(ws::Request const& request) // https://www.rfc-editor.org/rfc/rfc3875.html#section-4
{
	if (!is_regular_file(binPath))
		return 404;

	if (!is_executable_file(binPath))
		return 403;

	std::string cgiPath = request.getPath();

	size_t pointPos = cgiPath.find_last_of('.');

	std::cout << "pointPos: " << pointPos << std::endl;

	if (!pointPos || pointPos == std::string::npos)
		return 404;

	{
		std::string ext = cgiPath.substr(pointPos);
		std::cout << "ext: " << ext << std::endl;
		// if (ext.compare(".php") != 0)
		// {
		// 	std::cout << "CGI: only .php files are supported" << std::endl;
		// 	return 404;
		// }
		std::cout << "222ext: " << ext << std::endl;
	}

	cgiPath = root + "/" + cgiPath;

	if (!is_regular_file(cgiPath))
		return 404;

	if (!is_readable_file(cgiPath))
		return 403;

	setEnvp(cgiPath, request);

	return exec(cgiPath, request);
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

void CGI::setEnvp(std::string const& cgiPath, ws::Request const& request)
{
	std::map<std::string, std::string> const& reqenvp = request.getHeaders();

	for (std::map<std::string, std::string>::const_iterator it = reqenvp.begin(); it != reqenvp.end(); ++it)
		envp[headerToMetaData(it->first)] = it->second;

	envp["AUTH_TYPE"] = "";
	envp["CONTENT_LENGTH"] = envp["HTTP_CONTENT_LENGTH"];
	envp["SERVER_SOFTWARE"] = "webserv";

	if (envp["HTTP_HOST"].find(":") != std::string::npos)
	{
		envp["SERVER_NAME"] = envp["HTTP_HOST"].substr(0, envp["HTTP_HOST"].find(":"));
		envp["PORT"] = envp["HTTP_HOST"].substr(envp["HTTP_HOST"].find(":") + 1);
	}
	else
	{
		envp["SERVER_NAME"] = envp["HTTP_HOST"];
		envp["PORT"] = "";
	}

	envp["GATEWAY_INTERFACE"] = "CGI/1.1";
	envp["SERVER_PROTOCOL"] = "HTTP/1.1";

	std::cout << "request.getPath(): " << request.getPath() << std::endl;

	// if (request.getPath() != cgiPath)
	// {
	// 	console.log("[cgiPath]", cgiPath);
	// 	envp["PATH_INFO"] = request.getPath().substr(cgiPath.size());
	// 	envp["PATH_TRANSLATED"] = root + envp["PATH_INFO"];
	// }
	// else
	// {
	// 	envp["PATH_INFO"] = "";
	// 	envp["PATH_TRANSLATED"] = root + envp["PATH_INFO"];
	// }

	envp["QUERY_STRING"] = request.getQuery();
	envp["REMOTE_HOST"] = "";
	envp["REQUEST_METHOD"] = request.getMethod();
	envp["SCRIPT_NAME"] = cgiPath;
	envp["LES_PATH_ABDOMINO"] = cgiPath;
	envp["PATH_TRANSLATED"] = cgiPath;
	envp["PATH_INFO"] = cgiPath;
}

int CGI::exec(std::string cgiPath, ws::Request const& request)
{
	char tempOFile[] = "/tmp/outputFile_XXXXXX";

	int fdo = mkstemp(tempOFile);
	int fdi = 0;

	outputFile.assign(tempOFile);

	std::cout << "tempOFile: " << tempOFile << std::endl;

	if (fdo == -1)
		return 500; // internal server error

	if (!request.getBody().empty()) // IsHasBody()
	{
		char tempIfile[] = "/tmp/inputFile_XXXXXX";

		fdi = mkstemp(tempIfile);

		if (fdi == -1)
		{
			close(fdo);
			return 500; // internal server error
		}

		std::ofstream ifile(tempIfile);

		ifile << request.getBody();
		ifile.close();
	}

	pid_t pid = fork();
	if (!pid)
	{
		dup2(fdo, STDOUT_FILENO);
		dup2(fdi, STDIN_FILENO);
		// close(fdo);
		// close(fdi);

		int ret = execle(binPath.c_str(), binPath.c_str(), cgiPath.c_str(), NULL, mapToArray(envp));
		exit(1); // execve failed
	}

	close(fdo);
	close(fdi);

	console.log("@@@@@ HERE @@@@@");
	{ { { { { { { { {if (pid < 0) return 1;}}}}}}}}}
	// wait 5s for child process to finish
	console.log("@@@@@ waiting for child process to finish");
	int status;

	waitpid(pid, &status, 0);

	if (WIFEXITED(status))
	{
		console.log("@@@@@ WIFEXITED(status)");
	}
	else if (WIFSIGNALED(status))
	{
		console.log("@@@@@ WIFSIGNALED(status)");
	}
	else
	{
		console.log("@@@@@ WIFSTOPPED(status)");
	}

	console.log("Done!");

	// get timestamp from now
	

	//check for response headers 
	// unlink(tempOFile);
	// unlink(tempIfile);

	return 200;
}
