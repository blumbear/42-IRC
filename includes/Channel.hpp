#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sys/socket.h>

struct channelMod {
	bool inviteOnly;
	bool topicForOp;
	std::string password; //if password == "", password dosn't exist
	unsigned int userLimit; // if == 0 no user limit
};

struct clientInfo {
	bool isOp;
	int clientFd;
};

class Channel {
	private:
		std::map<std::string, clientInfo> _userMap; // nom de chaque utilisateur sur le channel et si oui ou non il est op
		std::string _name;
		channelMod _channelMod;
	public:


/* =========== Exception Handler =========== */

		class missingName: public std::exception {public :const char* what() const throw();};
		class passwordIncorect: public std::exception {public :const char* what() const throw();};
		class passwordTooLong: public std::exception {public :const char* what() const throw();};

/* ======= Constructor & Destructor ======= */

		Channel();
		Channel(std::string name);
		~Channel();

/* =========== Member Function =========== */

		void removedInviteOnly();
		void removedTopicForOp();
		void removedPassword();
		void removedUserLimit();

		void addInviteOnly();
		void addTopicForOp();
		void addPassword(std::string);
		void addUserLimit(unsigned int);

		void sendMessageToChannelUser(std::string);

		void addUser(std::string, int, bool);
		void removedUser(std::string);
};

