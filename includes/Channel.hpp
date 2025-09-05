#pragma once

#include <string>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <set>

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
		std::set<std::string> _inviteSet;
		unsigned int _numOfUser;
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

		void removedInviteOnly(clientId cData, std::string, unsigned int);
		void removedTopicForOp(clientId cData, std::string, unsigned int);
		void removedPassword(clientId cData, std::string, unsigned int);
		void removedUserLimit(clientId cData, std::string, unsigned int);
		void removedOp(clientId cData, std::string s, unsigned int);

		void addInviteOnly(clientId cData, std::string, unsigned int);
		void addTopicForOp(clientId cData, std::string, unsigned int);
		void addPassword(clientId cData, std::string s, unsigned int);
		void addUserLimit(clientId cData, std::string, unsigned int n);
		void addOp(clientId cData, std::string s, unsigned int);

		void addinvite(std::string name);

		void sendMessageToChannelUser(std::string msg, clientId cData, std::string cmd, bool prompt);
		void printChannelUser();

		void addUser(clientId cData, int clientFd, bool op);
		void removeUser(clientId cData);
		void removeUser(clientId cData, std::string msg);
		int removeUser(std::string name);

		void setName(std::string name) {_name = name;}
		void setTopic(std::string newTopic) {_topic = newTopic;}

		std::string getName() {return _name;}
		std::string getPassword() {return _channelMod.password;}
		bool getInvite() {return _channelMod.inviteOnly;}
		std::string getTopic() {return _topic;}
		bool getTopicOp() {return _channelMod.topicForOp;}

		bool find(std::string name) {return _userMap.count(name);}
		bool isInvite(std::string name) {return _inviteSet.count(name) != 0;}
		bool isOp(std::string name);
		void updateNick(std::string oldname, std::string newname);
};

