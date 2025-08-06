# 42-IRC

## How to use the program :

- Use the command `hostname -I | awk '{print $1}'` to obtein the IP address of the server.
- Next you can lunch the server with the command `./ircserv <port> <password>`. The server is now running on this terminal.
- Open an other terminal to connect a client to the server (tips: you van use the command `gnome-terminal` to create a terminal from another.).
- You have many way to connect a client, here we have two choice, nc and irrsi.

### nc :
- lunch the command `nc <server's Ip address> <server's port>` to connect the client to the server.
