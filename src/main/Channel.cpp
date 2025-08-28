#include "Channel.hpp"

/* ======= Constructor & Destructor ======= */

Channel::Channel() : _name("default") {}

Channel::Channel(std::string name) : _name(name) {
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

void Channel::removedInviteOnly() {_channelMod.inviteOnly = false;}

void Channel::removedTopicForOp() {_channelMod.topicForOp = false;}

void Channel::removedPassword() {_channelMod.password = "";}

void Channel::removedUserLimit() {_channelMod.userLimit = 0;}


void Channel::addInviteOnly() {_channelMod.inviteOnly = true;}

void Channel::addTopicForOp() {_channelMod.topicForOp = true;}

void Channel::addPassword(std::string newPassword) {
	if (newPassword.size() > 20)
		throw passwordTooLong();
	for (size_t i = 0; i < newPassword.size(); i++) {
		if (newPassword[i] != ' ') {
			_channelMod.password = newPassword;
			return ;
		}
	}
	throw passwordIncorect();
}

void Channel::addUserLimit(unsigned int n) {_channelMod.userLimit = n;}


void Channel::sendMessageToChannelUser(std::string msg, clientId cData, std::string cmd) {
	std::string toSend = ":" + cData._nickname + '!' + cData._username + "@tom " + cmd + " #" + _name + msg + "\r\n";
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
	clientInfo newclientInfo;
	newclientInfo.clientFd = clientFd;
	newclientInfo.isOp = op;
	_userMap[data._nickname] = newclientInfo;
	const std::string toSend(data._nickname + " join the channel.");
	sendMessageToChannelUser(" :" + toSend, data, "JOIN");
}

void Channel::removeUser(clientId data) {
	if (_userMap.count(data._nickname) == 0)
		throw Server::NotOnChannel();
	_userMap.erase(data._nickname);
	const std::string toSend(data._nickname + " quit the channel.");
	sendMessageToChannelUser(" :" + toSend, data, "PART");
}

void Channel::removeUser(clientId data, std::string msg) {
	if (_userMap.count(data._nickname) == 0)
		throw Server::NotOnChannel();
	_userMap.erase(data._nickname);
	const std::string toSend(msg);
	sendMessageToChannelUser(" :" + toSend, data, "PART");
}

int Channel::removeUser(std::string name) {
	std::cout << "-" << name << "-" << std::endl;
	if (_userMap.count(name) == 0)
		throw Server::NotOnChannel();
	int userFd = _userMap[name].clientFd;
	_userMap.erase(name);
	return userFd;
}

bool Channel::isOp(std::string name) {
	for (std::map<std::string, clientInfo>::iterator it = _userMap.begin(); it != _userMap.end(); it++) {
		if (it->first == name)
			return it->second.isOp;
	}
	throw Server::NotOnChannel();
}