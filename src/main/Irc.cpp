#include "Irc.hpp"

/* =========== Exception Handler =========== */

const char* Irc::Test::what() const throw() {return ("test");}

/* ======= Constructor & Destructor ======= */

Irc::Irc() {}

Irc::Irc(std::string password, uint16_t port) :
_password(password), _port(port)
{	
	throw(Test());
}

Irc::Irc(const Irc &other) {
	if (this != &other) {
		_password = other._password;
		_port = other._port;
	}
}

Irc::~Irc() {}
