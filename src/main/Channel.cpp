#include "Channel.hpp"

/* ======= Constructor & Destructor ======= */

Channel::Channel() : _name("default") {}

Channel::Channel(std::string name) : _name(name), _topic("Come chat."), _numOfUser(1) {
	_channelMod.inviteOnly = false;
	_channelMod.topicForOp = false;
	_channelMod.password = "";
	_channelMod.userLimit = 0;
	std::cout << "Channel " << _name << " is create." << std::endl;
}

Channel::~Channel() {}

Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		_name = other._name;
		_userMap = other._userMap;
		_channelMod = other._channelMod;
	}
	return (*this);
}

/* =========== Member Function =========== */

void Channel::removedInviteOnly(clientId cData, std::string, unsigned int) {_channelMod.inviteOnly = false; sendMessageToChannelUser("-i", cData, "MODE", true);}

void Channel::removedTopicForOp(clientId cData, std::string, unsigned int) {_channelMod.topicForOp = false; sendMessageToChannelUser("-t", cData, "MODE", true);}

void Channel::removedPassword(clientId cData, std::string, unsigned int) {_channelMod.password = ""; sendMessageToChannelUser("-k", cData, "MODE", true);}

void Channel::removedUserLimit(clientId cData, std::string, unsigned int) {_channelMod.userLimit = 0; sendMessageToChannelUser("-l", cData, "MODE", true);}

void Channel::removedOp(clientId cData, std::string name, unsigned int) {
	if (_userMap.count(name) == 0)
		throw Server::NotOnChannel();
	_userMap[name].isOp = false;
	sendMessageToChannelUser("-o " + name, cData, "MODE", true);
}

void Channel::addInviteOnly(clientId cData, std::string, unsigned int) {_channelMod.inviteOnly = true; sendMessageToChannelUser("+i", cData, "MODE", true);}

void Channel::addTopicForOp(clientId cData, std::string, unsigned int) {_channelMod.topicForOp = true; sendMessageToChannelUser("+t", cData, "MODE", true);}

void Channel::addPassword(clientId cData, std::string newPassword, unsigned int) {
	if (newPassword.size() > 20)
		throw passwordTooLong();
	for (size_t i = 0; i < newPassword.size(); i++) {
		if (newPassword[i] != ' ') {
			_channelMod.password = newPassword;
			sendMessageToChannelUser("+k " + newPassword, cData, "MODE", true);
			return ;
		}
	}
	throw passwordIncorect();
}

void Channel::addUserLimit(clientId cData, std::string, unsigned int n) {
	_channelMod.userLimit = n;
	std::ostringstream oss;
	oss << n;
	std::string str = oss.str();
	sendMessageToChannelUser("+l " + str, cData, "MODE", true);
}

void Channel::addOp(clientId cData, std::string name, unsigned int) {
	if (_userMap.count(name) == 0)
		throw Server::NotOnChannel();
	_userMap[name].isOp = true;
	sendMessageToChannelUser("+o " + name, cData, "MODE", true);
}


void Channel::addinvite(std::string name) {
	if (_inviteSet.count(name) != 0)
		throw Server::AlreadyInvite();
	else if (_userMap.count(name) != 0)
		throw Server::UserOnChan();
	_inviteSet.insert(name);
}


void Channel::sendMessageToChannelUser(std::string msg, clientId cData, std::string cmd, bool prompt) {
	std::string toSend;
	if (prompt) 
		toSend = ":" + cData._nickname + '!' + cData._username + "@tom " + cmd + " #" + _name + " " + msg + "\r\n";
	else
		toSend = msg;
	std::cout << "\033[36mSent in " << _name << "\033[0m :" << toSend;
	for (std::map<std::string, clientInfo>::iterator it = _userMap.begin(); it != _userMap.end(); ++it) {
		if (it->first != cData._nickname || cmd != "PRIVMSG")
			send(it->second.clientFd, toSend.c_str(), toSend.size(), 0);
	}
}

void Channel::printChannelUser() {
	for (std::map<std::string, clientInfo>::iterator it = _userMap.begin(); it != _userMap.end(); ++it) {
		std::cout << it->first << std::endl;
	}
}


void Channel::addUser(clientId data, int clientFd, bool op) {
	if (_numOfUser + 1 > _channelMod.userLimit && _channelMod.userLimit != 0)
		throw Server::ChanIsFull();
	else if (_channelMod.inviteOnly == true && _inviteSet.count(data._nickname) == 0)
		throw Server::ChanInviteOnly();
	if (find(data._nickname) == true)
		throw Server::UserOnChan();
	clientInfo newclientInfo;
	newclientInfo.clientFd = clientFd;
	newclientInfo.isOp = op;
	_userMap[data._nickname] = newclientInfo;
	_numOfUser++;
	const std::string toSend(data._nickname + " join the channel.");
	sendMessageToChannelUser(":" + toSend, data, "JOIN", true);
}

void Channel::removeUser(clientId data) {
	if (_userMap.count(data._nickname) == 0)
		throw Server::NotOnChannel();
	const std::string toSend(data._nickname + " quit the channel.");
	sendMessageToChannelUser(":" + toSend, data, "PART", true);
	_userMap.erase(data._nickname);
	_numOfUser--;
}

void Channel::removeUser(clientId data, std::string msg) {
	if (_userMap.count(data._nickname) == 0)
		throw Server::NotOnChannel();
	const std::string toSend(msg);
	sendMessageToChannelUser(":" + toSend, data, "PART", true);
	_userMap.erase(data._nickname);
	_numOfUser--;
}

int Channel::removeUser(std::string name) {
	if (_userMap.count(name) == 0)
		throw Server::NotOnChannel();
	int userFd = _userMap[name].clientFd;
	_userMap.erase(name);
	return userFd;
	_numOfUser--;
}

bool Channel::isOp(std::string name) {
	for (std::map<std::string, clientInfo>::iterator it = _userMap.begin(); it != _userMap.end(); it++) {
		if (it->first == name)
			return it->second.isOp;
	}
	return false;
}