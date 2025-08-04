#pragma once

struct Error {
	public:

    	enum class Code {
			TooFewArguments = -1,
			TooManyArguments = -2,
			InvalidInput = -3
    	};

		static std::string toString(Code code) {
       		switch (code) {
				case Code::TooFewArguments: return "Too few arguments provided";
				case Code::TooManyArguments: return "Too many arguments provided";
				case Code::InvalidInput: return "Invalid input";
				default: return "Unknown error";
        }
    }
	// (Optionnel) Affichage direct via ostream
	static void print(Code code) {
	    std::cerr << "error: " << toString(code) << std::endl;
		if (code >= -3 && code <= -1)
			std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
	}

};


