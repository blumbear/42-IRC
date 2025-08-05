#pragma once

/* ================= Lib Includes ================= */

#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <exception>
#include <stdint.h>

/* ================= Files includes ================= */

#include "error.hpp"

/* =================== Env Struct =================== */

class Irc {
	private:
		std::string	_password;
		uint16_t	_port;

	public:
/* =========== Exception Handler =========== */
		class Test: public std::exception {
			public :
				const char* what() const throw();
		};
/* ======= Constructor & Destructor ======= */

	Irc();
	Irc(std::string password = "", uint16_t port = 0);
	Irc(const Irc &other);
	~Irc();

};