#include "Server.hpp"


volatile sig_atomic_t g_shutdown = 0;

void freeData(int signum) {
	(void)signum;
	g_shutdown = 1;
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
	signal(SIGINT, freeData);
	try {
		Server server = Server(av[2], tmp);
		server.pollLoop();
	}
	catch (std::exception &e) {
		std::cout << "\033[32mError:\033[0m " << e.what() << std::endl;
		freeData(0);
	}
	return (EXIT_SUCCESS);
}
