#include "Server.hpp"

void Server::commandParse(const std::string& command, int clientFd) {
	std::map<std::string, void (Server::*)(int, const std::string &)> cmdMap;
	cmdMap["PASS"] = &Server::passCmd;
	cmdMap["NICK"] = &Server::nickCmd;
	cmdMap["USER"] = &Server::userCmd;
	cmdMap["CAP"] = &Server::capCmd;
	cmdMap["PING"] = &Server::pingCmd;
	cmdMap["JOIN"] = &Server::joinCmd;
	cmdMap["PRIVMSG"] = &Server::privmsgCmd;
	cmdMap["PART"] = &Server::partCmd;
	cmdMap["KICK"] = &Server::kickCmd;
	cmdMap["TOPIC"] = &Server::topicCmd;
	cmdMap["MODE"] = &Server::modeCmd;
	cmdMap["INVITE"] = &Server::inviteCmd;
	cmdMap["QUIT"] = &Server::quitCmd;

	std::vector<std::string> connexionCmd;
	connexionCmd.push_back("PASS");
	connexionCmd.push_back("CAP");
	connexionCmd.push_back("PING");
	
	size_t spacePos = command.find_first_of(' ');
	std::string function = command.substr(0, spacePos);
	if (std::find(connexionCmd.begin(), connexionCmd.end(), function) != connexionCmd.end()) {
		try {(this->*cmdMap[function])(clientFd, command);}
		catch (std::exception &e) {
			std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
			sendToClient(clientFd, e.what());
		}
	}
	else if (std::find(connexionCmd.begin(), connexionCmd.end(), function) == connexionCmd.end() && cmdMap.count(function) && _userMap[clientFd]._pass == false)
		throw PasswordIsNeeded();
	else if (cmdMap.count(function)) {
		try {(this->*cmdMap[function])(clientFd, command);}
		catch (std::exception &e) {
			std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
			sendToClient(clientFd, e.what());
		}
	}
	else throw UnknownCmdError();
	if (clientIsRegistered(clientFd) && _userMap[clientFd]._alreadyConnected == false && _userMap[clientFd]._pass == true) {
		std::string toSend = ":server 001 " + _userMap[clientFd]._nickname + " :Welcome to the IRC Network " + _userMap[clientFd]._nickname;
		sendToClient(clientFd, toSend);
		_userMap[clientFd]._alreadyConnected = true;
	}
}

void Server::passCmd(int clientFd, const std::string& command) {
	if (_userMap[clientFd]._alreadyConnected == true)
		throw AlreadyRegistered();
	if (_password == "")
		throw NoPasswordNeeded();
	if (_userMap[clientFd]._pass == true)
		return ;
	size_t spacePos = command.find_first_of(' ') + 1;
	if (spacePos == std::string::npos)
		throw CmdNeedMoreParam();
	std::string password = command.substr(spacePos);
	if (password != _password)
		throw WrongPassword();
	_userMap[clientFd]._pass = true;
}

void Server::nickCmd(int clientFd, const std::string& command) {
	std::string nickname = command.substr(command.find_first_of(' ') + 1);
	for (std::map<int, clientId>::iterator it = _userMap.begin(); it != _userMap.end(); ++it) {
		if (it->second._nickname == nickname)
			throw NickInUse();
	}
	if (nickname.size() > 15)
		throw NickTooLongError();
	if (_channelMap.empty() == false) {
		for (std::map<std::string, Channel>::iterator it = _channelMap.begin(); it != _channelMap.end(); it++) {
			if (it->second.find(_userMap[clientFd]._nickname) == true)
				it->second.updateNick(_userMap[clientFd]._nickname, nickname);
		}
	}
	sendToClient(clientFd, ":" + _userMap[clientFd]._nickname + "!" + _userMap[clientFd]._username + "@" + _serverHost + " NICK :" + nickname);
	_userMap[clientFd]._nickname = nickname;
}

void Server::userCmd(int clientFd, const std::string& command) {
	if (_userMap[clientFd]._alreadyConnected == true)
		throw AlreadyRegistered();
	size_t doubleDotPos = command.find_first_of(':');
	if (doubleDotPos == std::string::npos)
		throw UserCmdError();
	_userMap[clientFd]._realname = command.substr(doubleDotPos + 1);
	std::vector<std::string> tmpArray = split(command.substr(0, doubleDotPos), ' ');
	if (tmpArray.size() != 4)
		throw CmdNeedMoreParam();
	
	_userMap[clientFd]._username = tmpArray[1];
}

std::string Server::compareServOption(std::string option) {
	std::vector<std::string> tmpArray = split(_serverOption, ' ');
	std::vector<std::string> tmpArraybis = split(option, ' ');
	std::string res;

	for (size_t i = 0; i < tmpArray.size(); i++) {
		for (size_t j = 0; j < tmpArraybis.size(); j++)
			if (tmpArray[i] == tmpArraybis[j]) {
					res.append(tmpArray[i]);
					res.append(" ");
				}
	}

	return res;
}

void Server::capCmd(int clientFd, const std::string& command) {
	if (command.compare(0, 6, "CAP LS") == 0)
		sendToClient(clientFd, "CAP * LS :" + _serverOption);
	else if (command.compare(0, 7, "CAP REQ") == 0) {
		sendToClient(clientFd, "CAP * ACK :" + compareServOption(command.substr(0, command.find_first_of(':') + 1)));
	}
}

void Server::pingCmd(int clientFd, const std::string& command) {
	sendToClient(clientFd, "PONG " + command.substr(5));
}

void Server::joinCmd(int clientFd, const std::string& command) {
	size_t pos = command.find_first_of(' ');
	if (pos == std::string::npos)
		throw CmdNeedMoreParam();
	std::string cmd = command.substr(pos + 1);
	if (cmd[0] != '#')
		throw JoinFormatError();
	pos = cmd.find_first_of(' ');
	std::string channel;
	if (pos == std::string::npos)
		channel = cmd.substr(1);
	else
		channel = cmd.substr(1, pos - 1);
	std::string password = cmd.substr(pos + 1);
	if (channel.empty())
		throw CmdNeedMoreParam();
	if (_channelMap.count(channel) == 0) {
		for (size_t i = 0; i < channel.size(); i++){
			if (static_cast<std::string>("/;, \"\'%~()[]{}*+.").find(channel[i]) != std::string::npos || channel.size() > 15) {
				sendToClient(clientFd, ":"+_serverName+" 479 "+_userMap[clientFd]._nickname+" :Illegal channel name");
				throw JoinFormatError();
			}
		}
		_channelMap[channel] = Channel(channel);
		_channelMap[channel].addUser(_userMap[clientFd], clientFd, true);
	}
	else {
		try {
			if (password == _channelMap[channel].getPassword())
				_channelMap[channel].addUser(_userMap[clientFd], clientFd, false);
			else if (_channelMap[channel].getPassword() == "")
				_channelMap[channel].addUser(_userMap[clientFd], clientFd, false);
			else if (password != _channelMap[channel].getPassword())
				throw WrongPassword();
		} catch (std::exception &e) {
			std::cout << "\033[31mError\033[0m :"<<  e.what() << std::endl;
			sendToClient(clientFd, e.what());
		}
	}
}

void Server::privmsgCmd(int clientFd, const std::string& command) {
	size_t pos = command.find_first_of(':');
	if (pos == std::string::npos)
		throw PrivmsgFormatError();
	std::string cmd = command.substr(0, pos);
	size_t tmpPos = cmd.find_first_of(' ');
	if (tmpPos == std::string::npos)
		throw PrivmsgFormatError();
	std::string tmp = cmd.substr(tmpPos + 1);
	if (tmp[0] == '#') {
		tmpPos = tmp.find_first_of(' ');
		if (tmpPos == std::string::npos)
			tmpPos = tmp.size();
		std::string channel = tmp.substr(1, tmpPos - 1);
		if (_channelMap.count(channel)) {
				if (_channelMap[channel].find(_userMap[clientFd]._nickname) == false)
					throw NotOnChannel();
			_channelMap[channel].sendMessageToChannelUser(":" + command.substr(pos + 1), _userMap[clientFd], "PRIVMSG", true);
		}
		else throw ChannelNotFound();
	} else {
		tmpPos = tmp.find_first_of(' ');
		if (tmpPos == std::string::npos)
			tmpPos = tmp.size();
		std::string target = tmp.substr(0, tmpPos);
		for (std::map<int, clientId>::iterator it = _userMap.begin(); it != _userMap.end(); it++) {
			if (it->second._nickname == target) {
				sendToClient(it->first, ":" + _userMap[clientFd]._nickname + "!" + it->second._nickname + "@" + _serverHost + " PRIVMSG " + target + " :" + command.substr(pos + 1));
				return ;
			}
		}
		throw NoSuchNick();
	}
}

void Server::partCmd(int clientFd, const std::string& command) {
	std::vector<std::string> cmdVec = split(command, ' ');
	if (cmdVec.size() < 2)
		throw CmdNeedMoreParam();
	else if (cmdVec[1][0] != '#')
		throw JoinFormatError();
	std::string channel = cmdVec[1].substr(1);
	if (channel.empty())
		throw CmdNeedMoreParam();
	if (_channelMap.count(channel) == 0)
		throw NoSuchChannel();
	else if (_channelMap[channel].find(_userMap[clientFd]._nickname) == false)
		throw NotOnChannel();
	size_t dotpos = command.find_first_of(':');
	try {
		if (dotpos != std::string::npos)
			_channelMap[channel].removeUser(_userMap[clientFd], command.substr(dotpos + 1));
		else
			_channelMap[channel].removeUser(_userMap[clientFd]);
	} catch (std::exception &e) {
		std::cout << "\033[31m"<<  e.what() << "\033[0m" << std::endl;
		sendToClient(clientFd, e.what());
	}
}

void Server::kickCmd(int clientFd, const std::string& command) {
	size_t tmp = command.find_first_of(':');
	std::string msg = "";
	if (tmp != std::string::npos)
		msg = command.substr(tmp + 1);
	else
		tmp = command.size() - 1;
	size_t spacePos = command.find_first_of(' ');
	if (spacePos == std::string::npos)
		throw CmdNeedMoreParam();
	std::string buffer = command.substr(spacePos + 1, tmp);
	spacePos = buffer.find_first_of(' ');
	if (spacePos == std::string::npos)
		throw CmdNeedMoreParam();
	if (buffer[0] != '#') throw KickFormatError();
	std::string channel = buffer.substr(1, spacePos - 1);
	if (_channelMap.count(channel) == 0)
		throw NoSuchChannel();
	else if (_channelMap[channel].find(_userMap[clientFd]._nickname) == false)
		throw NotOnChannel();
	try {
		if (_channelMap[channel].isOp(_userMap[clientFd]._nickname) == false)
			throw ChanPrivNeeded();
		std::string userName = buffer.substr(spacePos + 1);
		spacePos = userName.find_first_of(' ');
		if (spacePos == std::string::npos)
			spacePos = userName.find_first_of(':');
		int userFd = _channelMap[channel].removeUser(userName.substr(0, spacePos));
		std::string kickMsg = command.substr(tmp + 1);
		sendToClient(userFd, ':'+_userMap[clientFd]._nickname+'!'+_userMap[userFd]._nickname+'@'+_serverHost+" KICK #"+channel+' '+_userMap[userFd]._nickname+" :"+kickMsg);
		_channelMap[channel].sendMessageToChannelUser(userName.substr(0, spacePos) + " :" + (kickMsg == "" ? userName.substr(0, spacePos) + " has beed kicked.":kickMsg), _userMap[clientFd], "KICK", true);
	} catch (std::exception &e) {
		std::cout << "\033[31m"<<  e.what() << "\033[0m" << std::endl;
		sendToClient(clientFd, e.what());
	}
}

void Server::topicCmd(int clientFd, const std::string& command) {
	size_t tmp = command.find_first_of(' ');
	if (tmp == std::string::npos)
		throw CmdNeedMoreParam();
	std::string cmd = command.substr(tmp + 1);
	if (cmd[0] != '#')
		throw TopicFormatError();
	tmp = cmd.find_first_of(' ');
	std::string channel;
	if (tmp == std::string::npos && cmd.find_first_of(':') == std::string::npos)
		channel = cmd.substr(1);
	else
		channel = cmd.substr(1, tmp - 1);
	if (_channelMap.count(channel) == 0)
		throw NoSuchChannel();
	else if (_channelMap[channel].find(_userMap[clientFd]._nickname) == false)
		throw NotOnChannel();
	tmp = cmd.find_first_of(':');
	if (tmp == std::string::npos)
		sendToClient(clientFd, ":server 332 " + _userMap[clientFd]._nickname + " #" + channel + " :" + _channelMap[channel].getTopic());
	else {
		if (_channelMap[channel].getTopicOp() == true
			&& _channelMap[channel].isOp(_userMap[clientFd]._nickname) == false)
				throw ChanPrivNeeded();
		_channelMap[channel].setTopic(cmd.substr(tmp + 1));
		_channelMap[channel].sendMessageToChannelUser(":" + _channelMap[channel].getTopic(), _userMap[clientFd], "TOPIC", true);
	}
}


void Server::modeCmd(int clientFd, const std::string& command) {
	std::map<std::string, void (Channel::*)(clientId, std::string, unsigned int)> addModeMap;
		addModeMap["i"] = &Channel::addInviteOnly;
		addModeMap["t"] = &Channel::addTopicForOp;
		addModeMap["k"] = &Channel::addPassword;
		addModeMap["o"] = &Channel::addOp;
		addModeMap["l"] = &Channel::addUserLimit;
	
	std::map<std::string, void (Channel::*)(clientId, std::string, unsigned int)> remModeMap;
		remModeMap["i"] = &Channel::removedInviteOnly;
		remModeMap["t"] = &Channel::removedTopicForOp;
		remModeMap["k"] = &Channel::removedPassword;
		remModeMap["o"] = &Channel::removedOp;
		remModeMap["l"] = &Channel::removedUserLimit;

	std::map<std::string, void (Channel::*)(clientId, std::string, unsigned int)> tmpMap;
	size_t tmp = command.find_first_of(' ');
	if (tmp == std::string::npos)
		throw CmdNeedMoreParam();
	std::string cmd = command.substr(tmp + 1);
	if (cmd[0] != '#')
		throw ModeFormatError();
	tmp = cmd.find_first_of(' ');
	if (tmp == std::string::npos)
		throw CmdNeedMoreParam();
	std::string channel = cmd.substr(1, tmp - 1);
	cmd = cmd.substr(tmp + 1);
	if (_channelMap.count(channel) == 0)
		throw NoSuchChannel();
	if (_channelMap[channel].find(_userMap[clientFd]._nickname) == false)
		throw NotOnChannel();
	if (_channelMap[channel].isOp(_userMap[clientFd]._nickname) == false)
		throw ChanPrivNeeded();

	if (cmd[0] == '-')
		tmpMap = remModeMap;
	else if (cmd[0] == '+')
		tmpMap = addModeMap;
	else
		throw ModeFormatError();

	tmp = cmd.find_first_of(' ');
	std::string arg;
	if (tmp == std::string::npos)
		arg = "";
	else
		arg = cmd.substr(tmp + 1);
	if (tmpMap.count(cmd.substr(1, 1)) != 0) {
		(_channelMap[channel].*(tmpMap[cmd.substr(1, 1)]))(_userMap[clientFd], arg, std::atoi(arg.c_str()));
	}
}

void Server::inviteCmd(int clientFd, const std::string& command) {
	if (_userMap.count(clientFd) == 0)
		throw NoSuchOnServer();
	size_t spacePos = command.find_first_of((' '));
	if (spacePos == std::string::npos)
		throw CmdNeedMoreParam();
	std::string cmd = command.substr(spacePos + 1);
	spacePos = cmd.find_first_of((' '));
	if (spacePos == std::string::npos)
		throw CmdNeedMoreParam();
	if (cmd[spacePos + 1] != '#')
		throw InviteFormatError();
	std::string channel = cmd.substr(spacePos + 2);
	if (_channelMap.count(channel) == 0)
		throw NoSuchChannel();
	std::string target = cmd.substr(0, spacePos);
	for (std::map<int, clientId>::iterator it = _userMap.begin(); it != _userMap.end(); it++) {
		if (it->second._nickname == target) {
			_channelMap[channel].addinvite(target);
			sendToClient(clientFd, ":" + _serverName + " 341 " + _userMap[clientFd]._nickname + " " + target + " :#" + channel);
			sendToClient(it->first, ":" + _userMap[clientFd]._nickname + "!" + it->second._nickname + "@" + it->second._nickname + " INVITE " + target + " :#" + channel);
			return ;
		}
	}
	throw NoSuchNick();
}

void Server::quitCmd(int clientFd, const std::string& command) {
	(void)command;
	for (std::map<std::string, Channel>::iterator it = _channelMap.begin(); it != _channelMap.end(); it++) {
		try {it->second.removeUser(_userMap[clientFd]._nickname);}
		catch (std::exception &e) {continue;}
	}
	_userMap.erase(clientFd);
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); it++) {
		if (it->fd == clientFd) {
			_fds.erase(it);
			close(clientFd);
			return ;
		}
	}
}