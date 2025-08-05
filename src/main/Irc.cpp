#include "Irc.hpp"

/* =========== Exception Handler =========== */

const char* Irc::UnknownError::what() const throw() {return ("Unknown error.");}

const char* Irc::ArgError::what() const throw() {return ("Argument of the constructor must be password and port.");}

const char* Irc::SocketError::what() const throw() {return ("The socket creation failed.");}

const char* Irc::BindError::what() const throw() {return ("Socket bind to server unsuccessful.");}

const char* Irc::FcntlError::what() const throw() {return ("Socket is still in blocking mode.");}

const char* Irc::ListenError::what() const throw() {return ("Server cant listen the socket.");}

/* ======= Constructor & Destructor ======= */

Irc::Irc() {throw (ArgError());}

Irc::Irc(std::string password, uint16_t port) :
_password(password), _port(port) {initSocket();}

Irc::Irc(const Irc &other) {
	if (this != &other) {
		_password = other._password;
		_port = other._port;
	}
}

Irc::~Irc() {}

/* ============ Private Function ============ */

void Irc::initSocket() {
	// Create the socket
	// AF_INET -> IPv4
	// SOCK_STREAM -> TCP
	// 0 -> used protocol
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
    	throw(SocketError());

	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
		throw ((FcntlError()));

	int opt = 1;
	// allows the port to be reused without waiting the end of TIMEWAIT
	// SOL_SOCKET -> level of the option (here is it for general option)
	// SO_REUSEADDR -> Socket Re use Address
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// define the server's address
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 listen every interface
	address.sin_port = htons(_port);

	// bind the socket to the address
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw (SocketError());

	// SOMAXCONN -> Socket Max Connexion (default 1024)
	if (listen(server_fd, SOMAXCONN) < 0)
		throw (ListenError());
}

/* ================= Loop ================= */

void Irc::pollLoop() {
	struct pollfd servFd;
	servFd.fd = _port;
}
