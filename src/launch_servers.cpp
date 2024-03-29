#include	"../inc/Server.hpp"
#include	"../inc/ServerException.hpp"
#include	"../inc/request_parser.hpp"
#include "../inc/launch_servers.hpp"
#include	<unistd.h>

#include <iterator>     // std::advance

#define	MAXLINE		1024	//By default, nginx sets client_header_buffer_size to 1 kilobyte (1024 bytes)
#define	CRLFx2		"\r\n\r\n"
#define	CONTINUE	"HTTP/1.1 100 Continue"

typedef vector<int>::iterator vecIntIt;

typedef sockaddr sa;
typedef sockaddr_in sa_in;

void	listening_connections(vector<Server> servers)
{
	fd_set			readfds, fdNow;
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
				cout << MAG << "Closing WebServ" << EOC << std::endl;
				break ;
			}
			if (!it->getClients().empty()){
				clients = it->getClients();
				for (vecIntIt client = clients.begin(); client != clients.end(); ++client){
					handle_sockets(*it, servers, fdNow, readfds, false, *client);
				}
			}
		}
	}
	close_sockets(servers);
	cout << MAG << "^...Servers closed...^" << EOC << "\n";
}

void accept_connection(int &socket, Server &server, fd_set &fds){
	int				client_fd;
	sa_in			addr;
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
	mapIntUri	clientRequest = server.getClientUri();
	clientRequest[client_fd] = URI();
	server.setClientUri(clientRequest);
}

bool	handle_sockets(Server &server, vector<Server> &servers, fd_set &ready, fd_set &all, bool sock_is_server, int client){
	int	socket = sock_is_server ? server.getSocket() : client;

	if (FD_ISSET(socket, &ready)){
		if (sock_is_server){
			cout << "Socket going to accept" << std::endl;
			accept_connection(socket, server, all);
			return (true);
		}
		read_connection(socket, server, server.getClientUri().at(client));
		if (server.getClientUri().at(client).getCloseConnection()){
			//keep connection alive ? si hay el header
			close_client_connection(client, server, servers, ready);
		}
	}
	return (false);
}

void read_connection(int &client, Server &server, URI &rq){				
	char request[MAXLINE];
	ssize_t	readed;

	if (server.getClientUri().at(client).getIsChunked() == false){
		memset(request, 0, MAXLINE);
		cerr << RED << "Going to read request" << EOC << std::endl;	
		readed = recv(client, request, sizeof(request), 0);
		if (readed <= 0){
			if (readed == 0){
				cerr << CYA << "Connection closed by client" << EOC << std::endl;	// No puede haber una lectura que sea 0 ??
			}
			else {	
				cerr << RED << "Error: Couldn't read from that client fd" << EOC << std::endl;
			}
			rq.setCloseConnection(true);
			return ;
		}
		string	tmp(request, readed);
		if (tmp.size() > 0){
			rq.setRequest(rq.getRequest() += tmp);
		}

		cout << "READED on port " << server.getPort() << ": [" << std::endl;
		string hidden(rq.getRequest());
		displayHiddenChars(hidden);
		cout << "]\n" << std::endl;

	}
	size_t	crlf = rq.getRequest().find(CRLFx2);
	if (crlf != string::npos){
		if (rq.getRequest().size() > crlf + 4){
			string body = rq.getRequest().substr(crlf + 4, rq.getRequest().size());
			rq.setBody(body);
			cout << CYA << "Body is+++++++>[" << 	rq.getBody() << std::endl << "]" << std::endl;
		}
		parse_rq(client, server, server.getClientUri().at(client)); 
	}
}

void parse_rq(int &client, Server &server, URI &rq){
	if (rq.getHeadersParsed() == false && 
			invalid_request(rq.getRequest().c_str(), rq)){
		cout << RED << "Error: Invalid request on server " << server.getPort() << " from client " << client << EOC << std::endl;
		return ;
	}

	if (rq.getExpectContinue() == true){
		send(client, CONTINUE, strlen(CONTINUE), 0);
		cout << CYA << "-100 sent" << EOC << std::endl;
		rq.setExpectContinue(false);
	}
	
	if (rq.getIsChunked()){
		if (read_chunked(client, rq)){
			cout << RED << "Error: Invalid Chunked request on server " << server.getPort() << " from client " << client << EOC << std::endl;
			//send bad request
			//close sockets?
			return ;
		}
	}
	if (rq.getIsChunked() == false){
		respond_connection(client, server, rq);
	}
}

void respond_connection(int &client, Server &server, URI &rq){

	string s_buff = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n";
	s_buff += rq.getBody();
	Response	response;
	///implement response on send
	if (send(client, s_buff.c_str(), strlen(s_buff.c_str()), 0) == -1){
		//close connection?
		cerr << RED << "Error: Couldn't send response for client " << client << EOC << std::endl;
		return ;
	}
	cout << RED << "Responding!!" << EOC << std::endl;
	server.getSocket(); //to avoid unused
	rq.setCloseConnection(true);
}
