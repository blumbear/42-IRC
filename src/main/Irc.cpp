#include "Irc.hpp"

/* =========== Exception Handler =========== */

const char* Server::UnknownError::what() const throw() {return ("Unknown error.");}

const char* Server::ArgError::what() const throw() {return ("Argument of the constructor must be password and port.");}

const char* Server::SocketError::what() const throw() {return ("The socket creation failed.");}

const char* Server::BindError::what() const throw() {return ("Socket bind to server unsuccessful.");}

const char* Server::FcntlError::what() const throw() {return ("Socket is still in blocking mode.");}

const char* Server::ListenError::what() const throw() {return ("Server cant listen the socket.");}

const char* Server::SetsockoptError::what() const throw() {return ("Server can't set socket option.");}

const char* Server::PollError::what() const throw() {return ("Poll failed to read the socket.");}

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
		std::cout << "New client connected: fd = " << client_fd << std::endl;	
	}
	else
		std::cerr << "New client failed to connect" << std::endl;
}

// void Server::commandParse(const std::string& command, int clientFd) {

// }

/* ================= Loop ================= */

void Server::pollLoop() {
	std::vector<pollfd> fds;
	pollfd tmp = {_serverFd, POLLIN, 0};
	fds.push_back(tmp);  // Server to check with accept()
	while (true) {

		int activity = poll(fds.data(), fds.size(), -1); // -1 = block
		if (activity < 0)
			throw (PollError());

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == _serverFd)
					newClient(fds);
				else {
					char buffer[1024];
					ssize_t bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (bytesRead > 0) {
						buffer[bytesRead] = '\0'; // Null-terminate
						std::string command(buffer);
						if (command.find('\n') == std::string::npos) {
							while (bytesRead > 0 ) {
								bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
								buffer[bytesRead] = '\0'; // Null-terminate
								command += buffer;
							}
						}
						std::cout << "Commande reçue du client fd " << fds[i].fd << " : " << command;
						// commandParse(command, fds[i].fd);
						
					} else if (bytesRead == 0) {
						std::cout << "Client déconnecté : fd = " << fds[i].fd << std::endl;
						close(fds[i].fd);
						fds.erase(fds.begin() + i);
						--i;
					} else {
						std::cerr << "Erreur de lecture sur le client fd " << fds[i].fd << std::endl;
					}
				}
			}
		}
	}
}
