#pragma once

#include <string>
#include <iostream>
#include <map>

struct channelMod {
	bool inviteOnly;
	bool topicForOp;
	std::string password; //if password == "", password dosn't exist
	unsigned int userLimit; // if == 0 no user limit
};

class Channel {
	private:
		std::map<std::string, bool> _userMap; // nom de chaque utilisateur sur le channel et si oui ou non il est op
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

};

