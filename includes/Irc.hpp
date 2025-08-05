#pragma once

/* ================= C Lib Includes ================= */

#include <stdint.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> 
#include <arpa/inet.h>

/* ================= C++ Lib Includes ================= */

#include <string>
#include <cstring>
#include <iostream>
#include <exception>
#include <cstdlib>

/* ================= Files includes ================= */

#include "error.hpp"

/* =================== Env Struct =================== */

class Irc {
	private:
		std::string	_password;
		uint16_t	_port;

/* ============ Private Function ============ */

		void initSocket();
		
	public:
/* =========== Exception Handler =========== */

		class UnknownError: public std::exception {public :const char* what() const throw();};
		class ArgError: public std::exception {public :const char* what() const throw();};
		class SocketError: public std::exception {public :const char* what() const throw();};
		class BindError: public std::exception {public :const char* what() const throw();};
		class FcntlError: public std::exception {public :const char* what() const throw();};
		class ListenError: public std::exception {public :const char* what() const throw();};

/* ======= Constructor & Destructor ======= */

	Irc();
	Irc(std::string password = "", uint16_t port = 0);
	Irc(const Irc &other);
	~Irc();

/* ================= Loop ================= */

	void pollLoop();
};
