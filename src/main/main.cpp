#include "Server.hpp"

void freeData() {
	return ;
}

int main(int ac , char **av) {
	if (ac < 3)
		return (Error::print(Error::TooFewArguments));
	else if (ac > 3)
		return (Error::print(Error::TooManyArguments));
	else if (!isdigit(av[1][0]))
			return (Error::print(Error::InvalidInput));
	int tmp = atoi(av[1]);
	if (tmp < 1 || tmp > 65535)
		return (Error::print(Error::InvalidPort));
	try {
		Server server = Server(av[2], tmp);
		server.pollLoop();
	}
	catch (std::exception &e) {
		std::cout << "\033[32mError:\033[0m " << e.what() << std::endl;
		freeData();
	}
	return (EXIT_SUCCESS);
}
