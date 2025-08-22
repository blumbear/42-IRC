#include "Server.hpp"

void Server::commandParse(const std::string& command, int clientFd) {
	std::map<std::string, void (Server::*)(int, const std::string &)> cmdMap;
	cmdMap["NICK"] = &Server::nickCmd;
	cmdMap["USER"] = &Server::userCmd;
	cmdMap["CAP"] = &Server::capCmd;
	cmdMap["PING"] = &Server::pingCmd;
	
	size_t spacePos = command.find_first_of(' ');
	
	if (cmdMap.count(command.substr(0, spacePos)))
		(this->*cmdMap[command.substr(0, spacePos)])(clientFd, command);
	if (clientIsRegistered(clientFd) && _userMap[clientFd]._allReadyConnect == false) {
		std::string toSend = ":server 001 " + _userMap[clientFd]._nickname + " :Welcome to the IRC Network " + _userMap[clientFd]._nickname;
		sendToClient(clientFd, toSend);
		_userMap[clientFd]._allReadyConnect = true;
	}
}

void Server::nickCmd(int clientFd, const std::string& command) {
	_userMap[clientFd]._nickname = command.substr(0, command.find_first_of(' ') + 1);
	if (_userMap[clientFd]._nickname.size() > 15) {
		_userMap[clientFd]._nickname = "";
		throw NickTooLongError();
	}
}

void Server::userCmd(int clientFd, const std::string& command) {
	size_t doubleDotPos = command.find_first_of(':');
	if (doubleDotPos == std::string::npos)
		throw UserCmdError();
	_userMap[clientFd]._realname = command.substr(doubleDotPos + 1);
	std::vector<std::string> tmpArray = split(command.substr(0, doubleDotPos), ' ');
	if (tmpArray.size() != 4)
		throw UserCmdError();
	
	_userMap[clientFd]._username = tmpArray[1];
}

std::string Server::compareServOption(std::string option) {
	std::vector<std::string> tmpArray = split(_serverOption, ' ');
	std::vector<std::string> tmpArraybis = split (option, ' ');
	std::string res;

	for (size_t i = 0; i < tmpArray.size(); i++) {
		for (size_t j = 0; j < tmpArraybis.size(); j++)
			if (tmpArray[i] == tmpArraybis[j])
				if (res.find(tmpArray[i]) == std::string::npos) {
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