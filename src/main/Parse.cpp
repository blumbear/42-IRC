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
	
	size_t spacePos = command.find_first_of(' ');
	
	if (cmdMap.count(command.substr(0, spacePos))) {
		try {(this->*cmdMap[command.substr(0, spacePos)])(clientFd, command);}
		catch (std::exception &e) {
			std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
			sendToClient(clientFd, e.what());
		}
	}
	else throw UnknownCmdError();
	if (clientIsRegistered(clientFd) && _userMap[clientFd]._alreadyConnected == false) {
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
	if (nickname.size() > 15) {
		throw NickTooLongError();
	}
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
	std::vector<std::string> cmdVec = split(command, ' ');
	if (cmdVec.size() < 2)
		throw CmdNeedMoreParam();
	else if (cmdVec[1][0] != '#')
		throw JoinFormatError();
	std::string tmp = cmdVec[1].substr(1);
	if (tmp.empty())
		throw CmdNeedMoreParam();
	if (_channelMap.count(tmp) == 0)
		_channelMap[tmp] = Channel(tmp);
	_channelMap[tmp].addUser(_userMap[clientFd], clientFd, true);
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
		size_t tmpP = tmp.find_first_of(' ');
		if (tmpP == std::string::npos)
			tmpP = tmp.size();
		if (_channelMap.count(tmp.substr(1, tmpP - 1)))
			_channelMap[tmp.substr(1, tmpP - 1)].sendMessageToChannelUser(command.substr(pos + 1), _userMap[clientFd], "PRIVMSG");
		else throw ChannelNotFound();
	} else {
		for (std::map<int, clientId>::iterator it = _userMap.begin(); it != _userMap.end(); it++) {
			if (it->second._nickname == tmp)
				sendToClient(it->first, "PRIVMSG :" + _userMap[clientFd]._nickname + " :" + command.substr(pos + 1));
		}
	}
}

void Server::partCmd(int clientFd, const std::string& command) {
	std::vector<std::string> cmdVec = split(command, ' ');
	if (cmdVec.size() < 2)
		throw CmdNeedMoreParam();
	else if (cmdVec[1][0] != '#')
		throw JoinFormatError();
	std::string tmp = cmdVec[1].substr(1);
	if (tmp.empty())
		throw CmdNeedMoreParam();
	if (_channelMap.count(tmp) == 0)
		_channelMap[tmp] = Channel(tmp);
	_channelMap[tmp].removeUser(_userMap[clientFd]);
}