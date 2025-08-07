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
#include <sstream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <map>
#include <vector>

/* ================= Files includes ================= */

#include "error.hpp"

/* =================== Env Struct =================== */

struct clientId {
	std::string _username;
	std::string _realname;
	std::string _nickname;
};

class Server {
	private:
		std::string	_password;
		uint16_t	_port;
		int			_serverFd;
		std::map<int, clientId> _userMap; // map of fd and nickname username

/* ============ Private Function ============ */

		void initSocket();
		void newClient(std::vector<pollfd>& fds);
		void commandParse(const std::string& command, int clientFd);
		bool clientIsRegistered(int clientFd);
		void handleCommand(std::vector<pollfd> fds, int i);
		void displayPrompt();
		
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

		class UserCmdError: public std::exception {public :const char* what() const throw();};

/* ======= Constructor & Destructor ======= */

	Server();
	Server(std::string password = "", uint16_t port = 0);
	Server(const Server &other);
	~Server();

/* ================= Loop ================= */

	void pollLoop();

/* ================= Utils ================= */

	std::vector<std::string> split(const std::string& str, char delimiter);
};
