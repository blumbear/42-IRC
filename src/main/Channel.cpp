#include "Channel.hpp"

/* ======= Constructor & Destructor ======= */

Channel::Channel() {throw missingName();}

Channel::Channel(std::string name) : _name(name) {
	_channelMod.inviteOnly = false;
	_channelMod.topicForOp = false;
	_channelMod.password = "";
	_channelMod.userLimit = 0;
}

Channel::~Channel() {}

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
