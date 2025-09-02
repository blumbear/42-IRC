#include "Server.hpp"

/* ======= Constructor & Destructor ======= */

Server::Server() {throw (ArgError());}

Server::Server(std::string password, uint16_t port) :
_password(password), _port(port), _serverOption("mutli-prefix server-time invite-notify"), _serverName("42_IRC") {
	initSocket();
}

Server::~Server() {}

/* ============ Private Function ============ */

void Server::initSocket() {
	// Create the socket
	// AF_INET -> IPv4
	// SOCK_STREAM -> TCP
	// 0 -> used protocol
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw(SocketError());

	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
		throw ((FcntlError()));

	int opt = 1;
	// allows the port to be reused without waiting the end of TIMEWAIT
	// SOL_SOCKET -> level of the option (here is it for general option)
	// SO_REUSEADDR -> Socket Re use Address
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw (SetsockOptError());

	// define the server's address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 listen every interface
	address.sin_port = htons(_port);

	// bind the socket to the address
	if (bind(_serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw (SocketError());

	getIpAddress();

	// SOMAXCONN -> Socket Max Connexion (default 1024)
	if (listen(_serverFd, SOMAXCONN) < 0)
		throw (ListenError());
	
	char hostname[250];
	if (gethostname(hostname, sizeof(hostname)) != 0)
		throw HostNameError();
	_serverHost = hostname;

}

void Server::newClient(std::vector<pollfd>& fds) {
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int client_fd = accept(_serverFd, (struct sockaddr*)&client_addr, &addr_len);
	if (client_fd >= 0) {
		pollfd tmp = {client_fd, POLLIN, 0};
		fds.push_back(tmp);
		clientId test;
		test._alreadyConnected = false;
		_userMap[client_fd] = test;
		std::cout << client_fd << " \033[32mIs connected\033[0m" << std::endl;
	}
	else
		std::cerr << "\033[31mClient failed to connect\033[0m" << std::endl;
}

void Server::sendToClient(int clientFd, const std::string& msg) {
	std::string toSend = msg + "\r\n";
	std::cout << "\033[34mServ Sent\033[0m :" << toSend;
	send(clientFd, toSend.c_str(), toSend.size(), 0);
}

bool Server::clientIsRegistered(int clientFd) {
	return (_userMap[clientFd]._username != "" && _userMap[clientFd]._nickname != "" && _userMap[clientFd]._realname != "");
}

static void strip_crlf(std::string &s) {
	if (!s.empty() && s[s.size()-1] == '\n') s.erase(s.size()-1);
	if (!s.empty() && s[s.size()-1] == '\r') s.erase(s.size()-1);
}

void Server::handleCommand(std::vector<pollfd> fds, int i) {
	char buffer[1024];
	ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate
		std::string command(buffer);
		std::cout << fds[i].fd << " \033[35mSend \033[0m:"<< command;
		if (std::count(command.begin(), command.end(), '\n') > 1) {
			std::vector<std::string> darray = split(command, '\n');
			for (std::vector<std::string>::iterator it = darray.begin(); it != darray.end(); ++it) {
				strip_crlf(*it);
				try {commandParse((*it), fds[i].fd);}
				catch (std::exception &e) {
					
					sendToClient(fds[i].fd, e.what());
				}
			}
		}
		else {
			try { commandParse(command.erase(command.find_last_not_of("\r\n ") + 1), fds[i].fd);}
			catch (std::exception &e) {
				std::cout << "\033[31mError\033[0m :"<<  e.what() << std::endl;
				sendToClient(fds[i].fd, e.what());
			}
		}
	}
	else if (bytesRead == 0) {
		std::cout << fds[i].fd << "\033[32m Disconnected\033[0m" << std::endl;
		close(fds[i].fd);
		fds.erase(fds.begin() + i);
	} else {
		std::cerr << "\033[31mError with the client\033[0m : " << fds[i].fd << std::endl;
	}
}

void Server::getIpAddress() {
	system("hostname -I | awk '{print $1}' > src/prompt/ip.txt");
	const std::string ipFile = "src/prompt/ip.txt";
	std::ifstream file(ipFile.c_str());
	std::stringstream ipS;
	ipS << file.rdbuf();
	system("rm src/prompt/ip.txt");
	_serverIp = ipS.str();
	_serverIp.erase(_serverIp.find_last_not_of(" \n\r") + 1);
}


void Server::displayPrompt() {
	std::ifstream file("src/prompt/serverHome.txt");
	if (file) {
		std::string line;
		while (std::getline(file, line)) {
			std::cout<< "\033[34m" << line << "\033[0m" << std::endl;
		}
		std::cout << "🚀 Server started successfuly!" << std::endl << std::endl;
		std::cout << "🗝️  Port: " << _port << std::endl;
		std::cout << "🔑 Password: " << _password << std::endl << std::endl;
		std::cout << "Server IRC started at " << _serverIp << ":" << _port << std::endl;
		file.close();
		file.open("src/prompt/logPrompt.txt");
		if (file) {
			while (std::getline(file, line))
				std::cout<< "\033[32m" << line << "\033[0m" << std::endl;
		}

	} else {
		std::cout << "Prompt file not found." << std::endl;
	}
}

void Server::sendPingToAllClients() {
	std::cout << "ping client" << std::endl;
	for (std::map<int, clientId>::iterator it = _userMap.begin(); it != _userMap.end(); ++it) {
		int clientFd = it->first;
		sendToClient(clientFd, "PING :" + _serverName);
	}
}

/* ================= Loop ================= */

void Server::pollLoop() {
	std::vector<pollfd> fds;
	pollfd tmp = {_serverFd, POLLIN, 0};
	fds.push_back(tmp);  // Server to check with accept()
	displayPrompt();
	while (true) {

		int activity = poll(fds.data(), fds.size(), -1); // -1 = block
		if (activity < 0)
			throw (PollError());

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _serverFd)
					newClient(fds);
				else
					handleCommand(fds, i);
			}
		}

	}
}

/* ================= Utils ================= */

std::vector<std::string> Server::split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		tokens.push_back(item);
	}
	return tokens;
}