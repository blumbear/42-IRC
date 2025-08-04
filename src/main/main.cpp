#include "irc.hpp"

int main(int ac , char **av) {
	if (ac < 3)
		return (Error::print(Error::Code::TooFewArguments));
	else if (ac > 3)
		return (Error::print(Error::Code::TooManyArguments));
	else if (isdigit(av[1][0])) {
		int tmp = atoi(av[1]);
		if (tmp < 0 || tmp > 65535)
			return (Error::print(Error::Code::InvalidInput));
	}
}