#include "Server.hpp"
#include "Channel.hpp"

/* =========== Server's Exception Handler =========== */

const char* Server::UnknownError::what() const throw() {return ("Unknown error.");}

const char* Server::ArgError::what() const throw() {return ("Argument of the constructor must be password and port.");}

const char* Server::SocketError::what() const throw() {return ("The socket creation failed.");}

const char* Server::BindError::what() const throw() {return ("Socket bind to server unsuccessful.");}

const char* Server::FcntlError::what() const throw() {return ("Socket is still in blocking mode.");}

const char* Server::ListenError::what() const throw() {return ("Server cant listen the socket.");}

const char* Server::SetsockoptError::what() const throw() {return ("Server can't set socket option.");}

const char* Server::PollError::what() const throw() {return ("Poll failed to read the socket.");}


const char* Server::UserCmdError::what() const throw() {return ("USER command wrong arguments.");}

const char* Server::NickTooLongError::what() const throw() {return ("Nick is too long.");}

const char* Server::UnknownCmdError::what() const throw() {return ("Unknown command or missing arguments.");}

const char* Server::JoinFormatError::what() const throw() {return (":server 461 <nick> JOIN :Not enough parameters");}

/* =========== Channel's Exception Handler =========== */

const char* Channel::missingName::what() const throw() {return ("Missing name to create a channel.");}

const char* Channel::passwordIncorect::what() const throw() {return ("The new password is incorect.");}

const char* Channel::passwordTooLong::what() const throw() {return ("The new password is too long.");}
