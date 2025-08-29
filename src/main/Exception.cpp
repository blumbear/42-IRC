#include "Server.hpp"
#include "Channel.hpp"

/* =========== Server's Exception Handler =========== */

const char* Server::UnknownError::what() const throw() {return ("Unknown error.");}

const char* Server::ArgError::what() const throw() {return ("Argument of the constructor must be password and port.");}

const char* Server::SocketError::what() const throw() {return ("The socket creation failed.");}

const char* Server::BindError::what() const throw() {return ("Socket bind to server unsuccessful.");}

const char* Server::FcntlError::what() const throw() {return ("Socket is still in blocking mode.");}

const char* Server::ListenError::what() const throw() {return ("Server cant listen the socket.");}

const char* Server::SetsockOptError::what() const throw() {return ("Server can't set socket option.");}

const char* Server::PollError::what() const throw() {return ("Poll failed to read the socket.");}


const char* Server::UserCmdError::what() const throw() {return ("USER command wrong arguments.");}

const char* Server::NoPasswordNeeded::what() const throw() {return ("No password is needed.");}

const char* Server::WrongPassword::what() const throw() {return ("Password is incorrect.");}

const char* Server::NickTooLongError::what() const throw() {return ("Nick is too long.");}


const char* Server::UnknownCmdError::what() const throw() {return ("Unknown command or missing arguments.");}

const char* Server::JoinFormatError::what() const throw() {return (":Join Format is not correct.");}

const char* Server::ChannelNotFound::what() const throw() {return (":The channel is not found.");}

const char* Server::KickFormatError::what() const throw() {return (":Kick format is not correct.");}

const char* Server::ModeFormatError::what() const throw() {return (":Mode format is not correct.");}

const char* Server::TopicFormatError::what() const throw() {return (":Topic format is not correct.");}


const char* Server::NoSuchNick::what() const throw() {return (":401 ERR_NOSUCHNICK");}

const char* Server::NoSuchChannel::what() const throw() {return (":403 ERR_NOSUCHCHANNEL");}

const char* Server::NoSuchOnServer::what() const throw() {return (":404 ERR_NOSUCHSERVER");}

const char* Server::PrivmsgFormatError::what() const throw() {return (":412 ERR_NOTEXTTOSEND");}

const char* Server::NickInUse::what() const throw() {return (":433 ERR_NICKNAMEINUSE");}

const char* Server::NotOnChannel::what() const throw() {return (":442 ERR_NOTONCHANNEL");}

const char* Server::UserOnChan::what() const throw() {return (":443 ERR_USERONCHANNEL");}

const char* Server::CmdNeedMoreParam::what() const throw() {return (":461 ERR_NEEDMOREPARAMS");}

const char* Server::AlreadyRegistered::what() const throw() {return (":462 ERR_ALREADYREGISTRED");}

const char* Server::ChanIsFull::what() const throw() {return (":471 ERR_CHANNELISFULL");}

const char* Server::UnknownMode::what() const throw() {return (":472 ERR_UNKNOWNMODE");}

const char* Server::ChanInviteOnly::what() const throw() {return (":473 ERR_INVITEONLYCHAN");}

const char* Server::ChanBadKey::what() const throw() {return (":475 ERR_BADCHANNELKEY");}

const char* Server::ChanPrivNeeded::what() const throw() {return (":482 ERR_CHANOPRIVSNEEDED");}

/* =========== Channel's Exception Handler =========== */

const char* Channel::missingName::what() const throw() {return ("Missing name to create a channel.");}

const char* Channel::passwordIncorect::what() const throw() {return ("The new password is incorect.");}

const char* Channel::passwordTooLong::what() const throw() {return ("The new password is too long.");}

