
	int Request::chunkedBody() //overcomplicated mess
	{
		char buffer[REQUEST_BUFFER_SIZE];

		static const std::string delim = "\r\n";
		static const size_t potentialMiss = delim.size() - 1;

		size_t prevSize = 0;

		// an alternative is chunk = _body if !_body.empty()
		while (true)
		{
			if (_body.find(delim, prevSize) != std::string::npos)
				break;
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				return 0;
			buffer[n] = '\0';

			prevSize = _body.size(); // only search in new bytes ?
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?
			_body.append(buffer, n);
		}

		size_t cl_end = _body.find(delim, prevSize);

		if (_body.find_first_not_of(CI_HEX) != cl_end)
			return 400;

		size_t chunk_length = strtol(_body.c_str(), NULL, 16);

		if (chunk_length == 0) // or is eof TODO
			return 400; // Bad request

		std::string	chunk = _body.substr(cl_end + 2); // from after \r\n to end of buffered _body
		_body.erase(cl_end); // _body should only have raw content

		prevSize = 0;
		while (chunk.size() < chunk_length)
		{
			if (chunk.find(delim, prevSize) != std::string::npos) // chunk.size() < chunk_length and delim found
				return 400;
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				break ; //experimenting
			buffer[n] = '\0';

			prevSize = chunk.size(); // only search in new bytes ?
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?
			chunk.append(buffer, n);
		}
		if (chunk.size() >= chunk_length)
		{
			if (chunk.find(delim, prevSize) != chunk_length) // chunk.size() > chunk_length and delim found || delim not found
				return 400;
		}
		// else read() break and everything fine 
		_body += chunk;
	}