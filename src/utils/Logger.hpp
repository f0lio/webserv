#pragma once

#include <fstream>
#include <string>

class Logger
{
public:
	Logger(const std::string fileName, bool append)
	{
		if (fileName == "")
			throw std::runtime_error("Invalid file name");
		if (append)
			_outputFile.open(fileName.c_str(), std::ios_base::app);
		else
			_outputFile.open(fileName.c_str());
		// if (_outputFile.is_open() == false)
		err_count = 0;
		write_count = 0;
		info_count = 0;
		warn_count = 0;
		debug_count = 0;
	}

	~Logger()
	{
		if (_outputFile.is_open())
			_outputFile.close();
	}

	void write(const std::string &msg)
	{
		_outputFile << msg;
		write_count++;
	}

	void info(const std::string &msg)
	{
		_outputFile << "INFO: " << msg << std::endl;
		info_count++;
	}

	void warn(const std::string &msg)
	{
		_outputFile << "WARN : " << msg << std::endl;
		warn_count++;
	}
	
	void debug(const std::string &msg)
	{
		_outputFile << "DEBUG: " << msg << std::endl;
		debug_count++;
	}

	void err(const std::string &msg)
	{
		_outputFile << "ERROR: " << msg << std::endl;
		err_count++;
	}

	// static Logger &getInstance()
	// {
	// 	static Logger instance("log.txt", true);
	// 	return instance;
	// }

private:
	std::ofstream _outputFile;
	size_t write_count;
	size_t err_count;
	size_t warn_count;
	size_t info_count;
	size_t debug_count;
} ;
