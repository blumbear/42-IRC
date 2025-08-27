#include "Server.hpp"

void Server::commandParse(const std::string& command, int clientFd) {
	std::map<std::string, void (Server::*)(int, const std::string &)> cmdMap;
	cmdMap["NICK"] = &Server::nickCmd;
	cmdMap["USER"] = &Server::userCmd;
	cmdMap["CAP"] = &Server::capCmd;
	cmdMap["PING"] = &Server::pingCmd;
	cmdMap["JOIN"] = &Server::joinCmd;
	// cmdMap["PRIVMSG"] = &Server::privmsgCmd;
	
	size_t spacePos = command.find_first_of(' ');
	
	if (cmdMap.count(command.substr(0, spacePos))) {
		try {(this->*cmdMap[command.substr(0, spacePos)])(clientFd, command);}
		catch (std::exception &e) {
			std::cout << "\033[31m" << e.what() << "\033[0m" << std::endl;
			sendToClient(clientFd, e.what());
		}
	}
	if (clientIsRegistered(clientFd) && _userMap[clientFd]._alreadyConnected == false) {
		std::string toSend = ":server 001 " + _userMap[clientFd]._nickname + " :Welcome to the IRC Network " + _userMap[clientFd]._nickname;
		sendToClient(clientFd, toSend);
		_userMap[clientFd]._alreadyConnected = true;
	}
}

void Server::nickCmd(int clientFd, const std::string& command) {
	_userMap[clientFd]._nickname = command.substr(command.find_first_of(' ') + 1);
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
		throw JoinFormatError();
	else if (cmdVec[1][0] != '#')
		throw JoinFormatError();
	std::string tmp = cmdVec[1].substr(1);
	if (tmp.empty())
		throw JoinFormatError();
	if (_channelMap.count(tmp) == 0)
		_channelMap[tmp] = Channel(tmp);
	_channelMap[tmp].addUser(_userMap[clientFd]._nickname, clientFd, true);
	_channelMap[tmp].printChannelUser();
}

// void Server::privmsgCmd(int clientFd, const std::string& command) {
// 	size_t pos = command.find_first_of(':');
// 	if (pos == std::string::npos)
// 		throw PrivmsgFormatError();
// 	std::string cmd = command.substr(0, pos);
// 	size_t tmpPos = cmd.find_first_of(' ');
// 	std::string tmp = cmd.substr(tmpPos + 1);
// 	if (tmp[0] == '#')
// 		_channelMap[tmp.substr(1)].sendMessageToChannelUser(_userMap[clientFd]._nickname + "" + command.substr(pos + 1));
// }