#pragma once

/* ================= C Lib Includes ================= */

#include <stdint.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <climits>
#include <ctime>

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
#include <algorithm>

/* ================= Files includes ================= */

#include "error.hpp"
#include "Channel.hpp"

/* =================== Env Struct =================== */

class Channel;

struct clientId {
	std::string _username;
	std::string _realname;
	std::string _nickname;
	bool _pass;
	bool _alreadyConnected;
};

class Server {
	private:
		std::string			_password;
		uint16_t			_port;
		int					_serverFd;
		std::string 		_serverIp;
		std::string 		_serverHost;
		std::string 		_serverOption;
		const std::string	_serverName;
		std::map<std::string, Channel> _channelMap;
		std::map<int, clientId>	_userMap; // map of fd and nickname username

/* ============ Private Function ============ */

		void initSocket();
		void newClient(std::vector<pollfd>& fds);
		bool clientIsRegistered(int clientFd);
		void handleCommand(std::vector<pollfd> fds, int i);
		void displayPrompt();
		void sendToClient(int client, const std::string& msg);
		void getIpAddress();
		void sendPingToAllClients();
		
		void commandParse(const std::string& command, int clientFd);
		std::string compareServOption(std::string);

		void passCmd(int, const std::string&);
		void nickCmd(int, const std::string&);
		void userCmd(int, const std::string&);
		void capCmd(int, const std::string&);
		void pingCmd(int, const std::string&);
		void joinCmd(int, const std::string&);
		void privmsgCmd(int, const std::string&);
		void partCmd(int, const std::string&);
		void kickCmd(int, const std::string&);
		void topicCmd(int, const std::string&);
		void modeCmd(int, const std::string&);
		void inviteCmd(int, const std::string&);
		void quitCmd(int, const std::string&);
	public:
/* =========== Exception Handler =========== */
		class UnknownError: public std::exception {public :const char* what() const throw();};
		class ArgError: public std::exception {public :const char* what() const throw();};
		class SocketError: public std::exception {public :const char* what() const throw();};
		class BindError: public std::exception {public :const char* what() const throw();};
		class FcntlError: public std::exception {public :const char* what() const throw();};
		class ListenError: public std::exception {public :const char* what() const throw();};
		class SetsockOptError: public std::exception {public :const char* what() const throw();};
		class PollError: public std::exception {public :const char* what() const throw();};
		class HostNameError: public std::exception {public :const char* what() const throw();};

		class UserCmdError: public std::exception {public :const char* what() const throw();};
		class NoPasswordNeeded: public std::exception {public :const char* what() const throw();};
		class WrongPassword: public std::exception {public :const char* what() const throw();};
		class NickTooLongError: public std::exception {public :const char* what() const throw();};
		class AlreadyInvite: public std::exception {public :const char* what() const throw();};
		
		class UnknownCmdError: public std::exception {public :const char* what() const throw();};
		class JoinFormatError: public std::exception {public :const char* what() const throw();};
		class ChannelNotFound: public std::exception {public :const char* what() const throw();};
		class KickFormatError: public std::exception {public :const char* what() const throw();};
		class ModeFormatError: public std::exception {public :const char* what() const throw();};
		class TopicFormatError: public std::exception {public :const char* what() const throw();};
		class InviteFormatError: public std::exception {public :const char* what() const throw();};

		class NoSuchNick: public std::exception {public :const char* what() const throw();};
		class NoSuchChannel: public std::exception {public :const char* what() const throw();};
		class NoSuchOnServer: public std::exception {public :const char* what() const throw();};
		class PrivmsgFormatError: public std::exception {public :const char* what() const throw();};
		class NickInUse: public std::exception {public :const char* what() const throw();};
		class NotOnChannel: public std::exception {public :const char* what() const throw();};
		class UserOnChan: public std::exception {public :const char* what() const throw();};
		class CmdNeedMoreParam: public std::exception {public :const char* what() const throw();};
		class AlreadyRegistered: public std::exception {public :const char* what() const throw();};
		class PasswordIsNeeded: public std::exception {public :const char* what() const throw();};
		class ChanIsFull: public std::exception {public :const char* what() const throw();};
		class UnknownMode: public std::exception {public :const char* what() const throw();};
		class ChanInviteOnly: public std::exception {public :const char* what() const throw();};
		class ChanBadKey: public std::exception {public :const char* what() const throw();};
		class ChanPrivNeeded: public std::exception {public :const char* what() const throw();};

/* ======= Constructor & Destructor ======= */

	Server();
	Server(std::string password = "", uint16_t port = 0);
	~Server();

/* ================= Loop ================= */

	void pollLoop();

/* ================= Utils ================= */

	std::vector<std::string> split(const std::string& str, char delimiter);
};
