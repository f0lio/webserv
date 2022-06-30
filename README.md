# Webserv
## HTTP Server in C++ using UNIX Sockets

### Usage:
```sh
$ make  #to build 
$ ./webserv #to run with default config file
$ ./webserv [config_file] #custom config file
$ ./webserv -t [config_file] #to check config file validity without running
```

### Features:
- HTTP 1.1 compliant 
- Never block and client can be bounced properly
- Non-blocking I/O operations
- Listen to multiple IPs
- Listen to multiple ports
- Methods: GET, POST, DELETE
- Able to upload files
- Able to serve a fully static website.
- Supports CGI (basic scripts)
- Custom error pages
- High availablity
- Resilient! Server never dies :slightly_smiling_face:
- `poll()` as the default event monitor

### Notes:
- Configuration file syntax is inspired by NGINX http server block.
- Most of the browser-related actions are done using Chrome (other browsers may not be compatible enough)

