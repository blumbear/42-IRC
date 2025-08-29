#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sys/socket.h>

#include "Server.hpp"

struct clientId;

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
		std::string _topic;
	public:


/* =========== Exception Handler =========== */

		class missingName: public std::exception {public :const char* what() const throw();};
		class passwordIncorect: public std::exception {public :const char* what() const throw();};
		class passwordTooLong: public std::exception {public :const char* what() const throw();};

/* ======= Constructor & Destructor ======= */

		Channel();
		Channel(std::string name);
		~Channel();
		Channel &operator=(const Channel &other);

/* =========== Member Function =========== */

		void removedInviteOnly(std::string, unsigned int);
		void removedTopicForOp(std::string, unsigned int);
		void removedPassword(std::string, unsigned int);
		void removedUserLimit(std::string, unsigned int);
		void removedOp(std::string s, unsigned int);

		void addInviteOnly(std::string, unsigned int);
		void addTopicForOp(std::string, unsigned int);
		void addPassword(std::string s, unsigned int);
		void addUserLimit(std::string, unsigned int n);
		void addOp(std::string s, unsigned int);

		void sendMessageToChannelUser(std::string, clientId, std::string, bool);
		void printChannelUser();

		void addUser(clientId, int, bool);
		void removeUser(clientId);
		void removeUser(clientId, std::string);
		int removeUser(std::string);

		void setName(std::string name) {_name = name;}
		void setTopic(std::string newTopic) {_topic = newTopic;}

		std::string getName() {return _name;}
		std::string getPassword() {return _channelMod.password;}
		std::string getTopic() {return _topic;}
		bool getTopicOp() {return _channelMod.topicForOp;}

		bool find(std::string name) {return _userMap.count(name);}
		bool isOp(std::string);
};

