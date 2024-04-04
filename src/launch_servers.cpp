#include	"../inc/Server.hpp"
#include	"../inc/ServerException.hpp"
#include	"../inc/request_parser.hpp"
#include "../inc/launch_servers.hpp"
#include	"../inc/responding.hpp"
#include	<unistd.h>


#define	MAXLINE		1024	//By default, nginx sets client_header_buffer_size to 1 kilobyte (1024 bytes)
#define	CRLFx2		"\r\n\r\n"
#define	HTTPS			"\x00\n\x00\n"
#define	CONTINUE	"HTTP/1.1 100 Continue"

typedef vector<int>::iterator vecIntIt;

typedef sockaddr sa;
typedef sockaddr_in sa_in;

void	setSockets(fd_set	&readfds, fd_set &writefds, vector<Server> servers){
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it){

		cout << MAG << "Socket---->" << it->getSocket() << EOC << std::endl;
		FD_SET(it->getSocket(), &readfds);
		if (it->getSocket() > it->getMax()){
			it->setMax(it->getSocket());
		}
		
		if (!it->getClients().empty()){
			vector<int> clients = it->getClients();
			for (vector<int>::iterator jt = clients.begin(); jt != clients.end(); ++jt ){
				cout << MAG << "\tClient---->" << *jt << EOC << std::endl;
				FD_SET(*jt, &readfds);
			}
		}
		
	}
}

void	listening_connections(vector<Server> servers)
{
	fd_set					readfds, writefds;
	vector<int>			clients;
  struct timeval	timeout;

	timeout.tv_sec = 20;
  timeout.tv_usec = 0;

	while (true){
		setSockets(readfds, writefds, servers);
		cout << MAG << "v...Waiting for connections...v" << EOC << std::endl;
		cout << MAG << "MAX->" << servers[0].getMax() << EOC << std::endl;

		/*
		for (vector<Server>::iterator	it = servers.begin(); it != servers.end(); ++it){
			cout << MAG << "Socket---->" << it->getSocket() << EOC << std::endl;
			if (!it->getClients().empty()){
				vector<int> clients = it->getClients();
				cout << MAG << "CLieeents---->" << EOC << std::endl;
				printContainer(clients);
			}
		}*/


		if (select(servers[0].getMax() + 1, &readfds, &writefds, NULL, &timeout) < 0){ //tiene que ser para read y write a la vez
			close_sockets(servers);
			perror("Socket errorrr");
			throw ServerException("Error when multiplexing with select()");
		}
		for (vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it){
			if (handle_sockets(*it, servers, readfds, writefds, true, 0)){
				break ;
			}
			if (!it->getClients().empty()){
				clients = it->getClients();
				for (vecIntIt client = clients.begin(); client != clients.end(); ++client){
					handle_sockets(*it, servers, readfds, writefds, false, *client);
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

bool	handle_sockets(Server &server, vector<Server> &servers, fd_set &readfds, fd_set &writefds, bool sock_is_server, int client){
	int	socket = sock_is_server ? server.getSocket() : client;

	if (FD_ISSET(socket, &readfds)){
		if (sock_is_server){
			cout << "Socket going to accept" << std::endl;
			accept_connection(socket, server, readfds);
			return (true);
		}
		read_connection(socket, server, server.getClientUri().at(client));

		if (server.getClientUri().at(client).getCloseConnection()){
			close_client_connection(client, server, servers, readfds);
		}
		else if (server.getClientUri().at(client).getGoingToResponse()){
			FD_CLR(client, &readfds);
			FD_SET(client, &writefds);
		}
	}
	if (FD_ISSET(socket, &writefds)){
		respond_connection(client, server,  server.getClientUri().at(client));
		close_client_connection(client, server, servers, writefds);
		//FD_CLR(client, &all); // ??
	}
	return (false);
}

void read_connection(int &client, Server &server, URI &rq){				
	char request[MAXLINE];
	ssize_t	readed;

	if (server.getClientUri().at(client).getIsChunked() == false){
		memset(request, 0, MAXLINE);
		cerr << RED << "Going to read request (client " << client << ")" << EOC << std::endl;	
		readed = recv(client, request, sizeof(request), 0);
		if (readed <= 0){
			if (readed == 0){
				cerr << CYA << "Connection closed by client (" << client << " fd)" << EOC << std::endl;	// No puede haber una lectura que sea 0 ??
			}
			else {	
				cerr << RED << "Error: Couldn't read from client (" << client << " fd)" << EOC << std::endl;
			}
			rq.setGoingToResponse (true);
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
	//size_t	https = rq.getRequest().find(HTTPS);
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
	if (rq.getHeadersParsed() == false && invalid_request(rq, server)){
		cout << RED << "/JOSEEEEEEE//////////STATUS CODE----> " << rq.getStatusCode()<< EOC << std::endl;
		cout << RED << "Error: Invalid request on server " << server.getPort() << " from client " << client << EOC << std::endl;
		//respond_connection(client, server, rq);
		rq.setGoingToResponse(true);
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
			//respond_connection(client, server, rq);
			rq.setGoingToResponse(true);
			return ;
		}
	}
	if (rq.getIsChunked() == false){
		//respond_connection(client, server, rq);
		rq.setGoingToResponse(true);
	}
}
