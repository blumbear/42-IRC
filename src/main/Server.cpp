#include "Server.hpp"

/* =========== Exception Handler =========== */

const char* Server::UnknownError::what() const throw() {return ("Unknown error.");}

const char* Server::ArgError::what() const throw() {return ("Argument of the constructor must be password and port.");}

const char* Server::SocketError::what() const throw() {return ("The socket creation failed.");}

const char* Server::BindError::what() const throw() {return ("Socket bind to server unsuccessful.");}

const char* Server::FcntlError::what() const throw() {return ("Socket is still in blocking mode.");}

const char* Server::ListenError::what() const throw() {return ("Server cant listen the socket.");}

const char* Server::SetsockoptError::what() const throw() {return ("Server can't set socket option.");}

const char* Server::PollError::what() const throw() {return ("Poll failed to read the socket.");}

const char* Server::UserCmdError::what() const throw() {return ("USER command wrong arguments.");}

/* ======= Constructor & Destructor ======= */

Server::Server() {throw (ArgError());}

Server::Server(std::string password, uint16_t port) :
_password(password), _port(port) {initSocket();}

Server::Server(const Server &other) {
	if (this != &other) {
		_password = other._password;
		_port = other._port;
	}
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
		throw (SetsockoptError());

	// define the server's address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 listen every interface
	address.sin_port = htons(_port);

	// bind the socket to the address
	if (bind(_serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw (SocketError());

	// SOMAXCONN -> Socket Max Connexion (default 1024)
	if (listen(_serverFd, SOMAXCONN) < 0)
		throw (ListenError());
}

void Server::newClient(std::vector<pollfd>& fds) {
	// Nouvelle connexion
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int client_fd = accept(_serverFd, (struct sockaddr*)&client_addr, &addr_len);
	if (client_fd >= 0) {
		pollfd tmp = {client_fd, POLLIN, 0};
		fds.push_back(tmp);
		_userMap[client_fd];
		std::cout << client_fd << " Is connected" << std::endl;
		sendToClient(client_fd, "Welcome on the IRC server!!");
	}
	else
		std::cerr << "Client failed to connect" << std::endl;
}

void Server::sendToClient(int clientFd, const std::string& msg) {
	std::string toSend = msg + "\r\n";
	send(clientFd, toSend.c_str(), toSend.size(), 0);
}

void Server::commandParse(const std::string& command, int clientFd) {

	if (clientIsRegistered(clientFd) == false) {
		if (command.compare(0, 5, "NICK ") == 0)
			_userMap[clientFd]._nickname = command.substr(5);
		else if (command.compare(0, 5, "USER ") == 0){
			std::vector<std::string> tmpArray = split(command, ':');
			std::vector<std::string> tmpArrayBis = split(tmpArray[0], ' ');
			tmpArray.erase(tmpArray.begin());
			tmpArray.insert(tmpArray.begin(), tmpArrayBis.begin(), tmpArrayBis.end());
			if (tmpArray.size() != 5)
				throw (UserCmdError());
			_userMap[clientFd]._username = tmpArray[1];
			_userMap[clientFd]._realname = tmpArray[4];
		}
		if (clientIsRegistered(clientFd) == true)
			std::cout << "client ID: " << clientFd << " nick: " << _userMap[clientFd]._nickname << " user: " << _userMap[clientFd]._username << " real name: " << _userMap[clientFd]._realname <<  std::endl;
	}
}

bool Server::clientIsRegistered(int clientFd) {
	return (_userMap[clientFd]._username != "" && _userMap[clientFd]._nickname != "");
}

void Server::handleCommand(std::vector<pollfd> fds, int i) {
	char buffer[1024];
	ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate
		std::string command(buffer);
		while (bytesRead > 0 && command.find('\n') == std::string::npos) {
			bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
			buffer[bytesRead] = '\0'; // Null-terminate
			command += buffer;
		}
		if (command != "\n") {
			if (std::count(command.begin(), command.end(), '\n') > 1) {
				std::vector<std::string> darray = split(command, '\n');
				for (size_t i = 0; i < darray.size(); i++) {
					try {commandParse(darray[i].erase(darray[i].find_last_not_of("\r\n ") + 1), fds[i].fd);}
					catch (std::exception &e) {std::cout << e.what() << std::endl;}
				}
			}
			std::cout << fds[i].fd << " Send : "<< command;
			try { commandParse(command.erase(command.find_last_not_of("\r\n ") + 1), fds[i].fd);}
			catch (std::exception &e) {std::cout << e.what() << std::endl;}
		}
	}
	else if (bytesRead == 0) {
		std::cout << fds[i].fd << " Disconnected" << std::endl;
		close(fds[i].fd);
		fds.erase(fds.begin() + i);
	} else {
		std::cerr << "Error with the client : " << fds[i].fd << std::endl;
	}
}

std::string getIpAddress() {
	std::string ip;
	FILE* fp = popen("hostname -I | awk '{print $1}'", "r");
	if (fp) {
		char buffer[128];
		if (fgets(buffer, sizeof(buffer), fp) != NULL) {
			ip = buffer;
			ip.erase(ip.find_last_not_of(" \n\r") + 1);
		}
		pclose(fp);
	}
	return ip;
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
		std::cout << "Server IRC started at " << getIpAddress() << ":" << _port << std::endl;
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