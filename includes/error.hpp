#pragma once

/* ========== Error Handler ========== */
struct Error {
	enum Code {
		Default = -1,
		TooFewArguments = -2,
		TooManyArguments = -3,
		InvalidInput = -4,
		InvalidPort = -5
	};

	static std::string toString(Code code) {
		switch (code) {
			case TooFewArguments: return "Too few arguments provided.";
			case TooManyArguments: return "Too many arguments provided.";
			case InvalidInput: return "Invalid input.";
			case InvalidPort: return "Invalid port, he need to be between 1 and 65535."; // Value on unsigned 16 bite and 0 is reserved
			default: return "Unknown error.";
		}
	}
	// (Optionnel) Affichage direct via ostream
	static int print(Code code) {
		std::cerr << "error: " << toString(code) << std::endl;
		if (code >= -3 && code <= -1)
			std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (EXIT_SUCCESS);
	}

};


