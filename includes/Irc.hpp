#pragma once

/* ================= C Lib Includes ================= */

#include <stdint.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <unistd.h>

/* ================= C++ Lib Includes ================= */

#include <string>
#include <cstring>
#include <iostream>
#include <exception>
#include <cstdlib>
#include <map>
#include <vector>

/* ================= Files includes ================= */

#include "error.hpp"

/* =================== Env Struct =================== */

class Server {
	private:
		std::string	_password;
		uint16_t	_port;
		int			_serverFd;
		std::map<std::string, std::string> _userMap;

/* ============ Private Function ============ */

		void initSocket();
		void newClient(std::vector<pollfd>& fds);
		
	public:
/* =========== Exception Handler =========== */

		class UnknownError: public std::exception {public :const char* what() const throw();};
		class ArgError: public std::exception {public :const char* what() const throw();};
		class SocketError: public std::exception {public :const char* what() const throw();};
		class BindError: public std::exception {public :const char* what() const throw();};
		class FcntlError: public std::exception {public :const char* what() const throw();};
		class ListenError: public std::exception {public :const char* what() const throw();};
		class SetsockoptError: public std::exception {public :const char* what() const throw();};
		class PollError: public std::exception {public :const char* what() const throw();};

/* ======= Constructor & Destructor ======= */

	Server();
	Server(std::string password = "", uint16_t port = 0);
	Server(const Server &other);
	~Server();

/* ================= Loop ================= */

	void pollLoop();
};
