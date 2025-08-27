#include "Channel.hpp"

/* ======= Constructor & Destructor ======= */

Channel::Channel() : _name("default") {}

Channel::Channel(std::string name) : _name(name) {
	_channelMod.inviteOnly = false;
	_channelMod.topicForOp = false;
	_channelMod.password = "";
	_channelMod.userLimit = 0;
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


void Channel::sendMessageToChannelUser(std::string msg) {
	std::string toSend = "#" + _name + " :"+ msg + "\r\n";
	std::cout << "\033[36mSent in " << _name << "\033[0m :" << toSend;
	for (std::map<std::string, clientInfo>::iterator it = _userMap.begin(); it != _userMap.end(); ++it) {
		send(it->second.clientFd, toSend.c_str(), toSend.size(), 0);
	}
}

void Channel::printChannelUser() {
	for (std::map<std::string, clientInfo>::iterator it = _userMap.begin(); it != _userMap.end(); ++it) {
		std::cout << it->first << std::endl;
	}
}


void Channel::addUser(std::string name, int clientFd, bool op) {
	clientInfo newclientInfo;
	newclientInfo.clientFd = clientFd;
	newclientInfo.isOp = op;
	_userMap[name] = newclientInfo;
	const std::string toSend(name + " join the channel.");
	sendMessageToChannelUser(toSend);
}

void Channel::removedUser(std::string name) {_userMap.erase(name);}
