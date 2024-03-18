#include	"../inc/Server.hpp"
#include	"../inc/ServerException.hpp"
#include	"../inc/request_parser.hpp"
#include "../inc/launch_servers.hpp"
#include	<unistd.h>

#define	MAXLINE	1024	//By default, nginx sets client_header_buffer_size to 1 kilobyte (1024 bytes)

typedef sockaddr sa;
typedef sockaddr_in sa_in;

void	listening_connections(vector<Server> servers)
{
	fd_set	readfds, fdNow;
	vector<int>	clients;

	FD_ZERO(&readfds);
	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it){
		FD_SET(it->getSocket(), &readfds);
		if (it->getSocket() > it->getMax()){
			it->setMax(it->getSocket());
		}
	}

	while (true){
		fdNow = readfds;
		cout << MAG << "v...Waiting for connections...v" << EOC << std::endl;
		if (select(servers[0].getMax() + 1, &fdNow, NULL, NULL, NULL) < 0){
			close_sockets(servers);
			throw ServerException("Error when multiplexing with select()");
		}
		for (vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it){
			if (handle_sockets(*it, servers, fdNow, readfds, true, 0)){
				break ;
			}
			if (!it->getClients().empty()){
				clients = it->getClients();
				for (vector<int>::iterator client = clients.begin(); client != clients.end(); ++client){
					handle_sockets(*it, servers, fdNow, readfds, false, *client);
				}
			}
		}
	}
	close_sockets(servers);
	cout << MAG << "^...Servers closed...^" << EOC << "\n";
}

void accept_connection(int &socket, Server &server, fd_set &fds){
	int	client_fd;
	sa_in addr;
	socklen_t addr_len = sizeof(addr);

	client_fd = accept(socket, (sa *)&addr, &addr_len);
	if (server.setClients(client_fd) == false){
		cerr << RED << "Error: Couldn't create fd for client" << EOC << std::endl;
		return ;
	}
	if (server.getMax() < client_fd){
		server.setMax(client_fd);
	}
	FD_SET(client_fd, &fds);
}

bool	handle_sockets(Server &server, vector<Server> servers, fd_set &ready, fd_set &all, bool sock_is_server, int client){
	int	socket = sock_is_server ? server.getSocket() : client;

	if (FD_ISSET(socket, &ready)){
		if (sock_is_server){
			cout << "Socket going to accept" << std::endl;
			accept_connection(socket, server, all);
			return (true);
		}
		read_connection(socket, server);
		close(client);
		server.remove_client(client);
		calculate_max(servers);
		FD_CLR(socket, &all);
	}
	return (false);
}

void read_connection(int &client, Server &server){				
			char request[MAXLINE];
			ssize_t	readed;
			string	rq("");

			while (rq.find("\r\n\r\n") == string::npos){
				memset(request, 0, MAXLINE);
				if ((readed = recv(client, request, sizeof(request), 0)) <= 0){
					if (readed == 0){
						cerr << CYA << "Connection closed by client" << EOC << std::endl;	
					}
					else {	
						cerr << RED << "Error: Couldn't read from that client fd" << EOC << std::endl;
					}
					break ;
				}
				string	tmp(request, readed);
				rq += tmp;
			}
			cout << "READED on port " << server.getPort() << ": [" << displayHiddenChars(rq) << "]" << std::endl;
			if (rq.size() > 0){
				respond_connection(client, server, rq);
			}
}

void respond_connection(int &client, Server &server, string &request){
	URI	rq_info;
	if (invalid_request(request.c_str(), rq_info)){
		cout << RED << "Error: Invalid request on server " << server.getPort() << " from client " << client << EOC << std::endl;
		return ;
	}
	cout << rq_info;
	std::string s_buff = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: 13\r\n\n<h1>Hola</h1>";
	if (send(client, s_buff.c_str(), strlen(s_buff.c_str()), 0) == -1){
		cerr << RED << "Error: Couldn't send response for client " << client << EOC << std::endl;
		return ;
	}
}
