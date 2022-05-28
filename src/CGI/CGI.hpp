#pragma once

#include "CGI_utils.hpp"

namespace	ws
{
	struct Location // I give up, I don't know how to do it without redeclaring
	{
		std::string path;
		std::map<std::string, t_vec_str> config;
	};

	// only one per binary (php, python, ruby, etc.) from which run() is called with the script directory? not sure
	class CGI
	{

		private:
			std::map<std::string, std::string>	envp;
			std::string							name;
			std::string							binPath;

		public:
			CGI() {}
			~CGI() {}

			int	run(std::string cgiPath)
			{

				if (!file_exists(cgiPath))
					return 1; // file not found?

				// setup the environment variables

				return this->exec(cgiPath);
			}

		private:
			int	exec(std::string cgiPath)
			{
				pid_t	pid = fork();

				if (pid > 0)
					return 1;
				if (!pid)
				{
					int	ret = execle(binPath.c_str(), binPath.c_str(), cgiPath.c_str(), NULL, map_to_envp(envp));
					exit(1); // execve failed
				}
				int	status;
				waitpid(pid, &status, 0);
				if (WIFEXITED(status))
					return WEXITSTATUS(status);
				return 0;
			}
			// std::string const &getCGI() const;
	};
}