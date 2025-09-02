# 42-IRC

## How to use the program :

- Use the command `hostname -I | awk '{print $1}'` to obtein the IP address of the server.
- Next you can lunch the server with the command `./ircserv <port> <password>`. The server is now running on this terminal.
- Open an other terminal to connect a client to the server. (tips: you van use the command `gnome-terminal` to create a terminal from another.)
- You have many way to connect a client, here we have two choice, nc and irrsi.

### nc :
- lunch the command `nc <server's Ip address> <server's port>` to connect the client to the server.
- next use the command `PASS <password>`, `NICK <nickname>` and `USER <username> <hostname> <servername> :<realname>` to being identificate on this server. (help: the hostname is historicaly 0 and the server's name is * if you don't know it or don't use it.)

### irssi :
- lunch the command `/connect localhost <port> <password>`. IRSSI will automaticaly use the command PASS, USER and NICK.

### command :
The list of command will follow the order `nc's command` | `irssi's command`.
- `PASS <password>`
- `USER <username> <hostname> <servername> :<realname>` |
- `NICK <nickname>` | `/nick <nickname>` |
- `JOIN #<channel>` | `/join channel`
- `PART #channel` | `/part`
- `PRIVMSG #<channel> :<message>` | `<message>` (in a channel)
- `PRIVMSG <target> :<message>`
- `INVITE <target> #<channel>` | `/invite <target>` (in a channel)
- `TOPIC`