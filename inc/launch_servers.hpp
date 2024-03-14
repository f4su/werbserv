#ifndef	LAUNCH_SERVERS_HPP
# define	LAUNCH_SERVERS_HPP
#include	"Server.hpp"

#define RED "\033[1;31m"
#define CYA	"\033[36m"
#define MAG	"\033[1;35m"
#define EOC	"\033[0m"

using std::vector;
using std::string;
using std::cout;
using std::cerr;

//	../src/launch_servers.cpp
void		listening_connections(std::vector<Server> servers);
void		accept_connection(int &socket, Server &server, fd_set &fds);
bool		handle_sockets(Server &server, vector<Server> servers, fd_set &ready, fd_set &all, bool sock_is_server, int client);
void		read_connection(int &client, Server &server);
void		respond_connection(int &socket, Server &server, string &request);

//	../src/launch_servers_utils.cpp
string	displayHiddenChars(string& str);
void		prepare_sockets(vector<Server> &servers);
void		calculate_max(vector<Server> &servers);
void		close_sockets(vector<Server> &servers);

#endif

