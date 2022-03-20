# Webserv
## HTTP Server in C++ using UNIX Sockets

### Usage:
```sh
$ make  #to build 
$ ./webserv [configuration file]
```

### Features:
- [ ] HTTP 1.1 compliant 
- [ ] Never block and client can be bounced properly
- [ ] Non-blocking I/O operations
- [ ] Listen to multiple ports
- [ ] Able to upload files
- [ ] Able to serve a fully static website.
- [ ] Supports CGI
- [ ] Accurate HTTP response status codes
- [ ] Compatible with Chrome Web Browser 
- [ ] Default error pages if none are provided
- [ ] Methods:
  - [ ] GET
  - [ ] POST
  - [ ] DELETE
- [ ] High availablity
- [ ] All read/write operations goes through poll() (or equivalent)
- [ ] Resilient! Server never dies.


### Allowed functions:
```
htons, htonl, ntohs, ntohl, select, poll, epoll
(epoll_create, epoll_ctl, epoll_wait), kqueue
(kqueue, kevent), socket, accept, listen, send,
recv, bind, connect, inet_addr, setsockopt,
getsockname, fcnt
```
