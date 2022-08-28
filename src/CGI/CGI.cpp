
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
	{
		console.err("CGI::run: binPath " + binPath + " is not a file");
		return 404;
	}

	if (!is_executable_file(binPath))
	{
		console.err("CGI::run: binPath " + binPath + " is not executable");
		return 403;
	}

	std::string cgiPath = request.getPath();

	size_t pointPos = cgiPath.find_last_of('.');

	// std::cout << "pointPos: " << pointPos << std::endl;

	if (!pointPos || pointPos == std::string::npos)
	{
		console.err("CGI::run: cgiPath " + cgiPath + " has no extension");
		return 404;
	}

	{
		std::string ext = cgiPath.substr(pointPos);
		if (ext.compare(".py") != 0)
			return 404;
	}

	cgiPath = root + "/" + cgiPath;

	if (!is_regular_file(cgiPath))
	{
		console.err("CGI::run: cgiPath " + cgiPath + " is not a file");
		return 404;
	}

	if (!is_readable_file(cgiPath))
	{
		console.err("CGI::run: cgiPath " + cgiPath + " is not readable");
		return 403;
	}

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

	// std::cout << "request.getPath(): " << request.getPath() << std::endl;

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

	// std::cout << "tempOFile: " << tempOFile << std::endl;

	if (fdo == -1)
	{
		console.err("CGI::exec: mkstemp failed");
		return 500; // internal server error
	}

	char tempIfile[] = "/tmp/inputFile_XXXXXX";
	if (!request.getBody().empty()) // IsHasBody()
	{

		fdi = mkstemp(tempIfile);

		if (fdi == -1)
		{
			console.err("CGI::exec: mkstemp failed");
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
		char** pth = paths(binPath, cgiPath);

		mapToArray(envp);

		int ret = execvp(pth[0], pth);
		console.err("CGI::exec: execve failed");
		perror("CGI::exec: execve failed");
		exit(ret);
		// exit(2);
	}

	close(fdo);
	close(fdi);

	if (pid < 0)
		return 1;

	int status;
	time_t start = time(NULL);
	while (time(NULL) - start < 3)
	{
		pid_t ret = waitpid(pid, &status, WNOHANG);
		if (ret == pid)
			break;
		usleep(100);
	}

	if (!WIFEXITED(status))
	{
		console.err("CGI::exec: child process did not exit");
		return 408; // internal server error
	}
	else
	{
		int ret = WEXITSTATUS(status);
		if (ret != 0)
		{
			console.err("CGI::exec: child process exited with status " + SSTR(ret));
			return 500; // internal server error
		}
	}

	// unlink(tempOFile);

	if (!request.getBody().empty()) // IsHasBody()
		unlink(tempIfile);

	return 200;
}
