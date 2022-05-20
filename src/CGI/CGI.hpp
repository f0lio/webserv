#pragma once

#include "CGI_utils.hpp"

namespace	ws
{
	struct Location // I give up, I don't know how to do it without redeclaring
	{
		std::string path;
		std::map<std::string, t_vec_str> config;
	};
	class CGI
	{

		private:
			Location const						location;
			std::map<std::string, std::string>	envp;

		public:
			CGI(Location const &location) : location(location) {}
			~CGI() {}

			int	run(std::string binPath)
			{
				binPath = location.path + "/" + binPath;

				if (!file_exists(binPath))
					return 1; // file not found?

				// setup the environment variables

				return this->exec(binPath);
			}

		private:
			int	exec(std::string path)
			{
				pid_t	pid = fork();

				if (pid > 0)
					return 1;
				if (!pid)
				{
					int	ret = execve(path.c_str(), paths(path), map_to_envp(envp));
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