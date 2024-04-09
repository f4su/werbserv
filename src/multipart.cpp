
#include	"../inc/request_parser.hpp"
#include	"../inc/launch_servers.hpp"
#include	"../inc/headers.hpp"
#include <sys/socket.h>
#include <sstream>
#include <algorithm>

using	std::cout;
using	std::cerr;

#define	MAXLINE					1024 //By default, nginx sets client_header_buffer_size to 1 kilobyte (1024 bytes)
#define	CRLF						"\r\n"

bool invalid_multipart(URI &rq, string &boundaryStart, string &boundaryEnd){
	string				body = rq.getBody(), line;
	std::stringstream	ss(body);
	size_t				lineInd = 0;
	size_t				endInd = 0;

	if (body.find(boundaryEnd) == string::npos || (body.find(boundaryEnd) != body.rfind(boundaryEnd))){
		rq.setStatusCode(STATUS_400);
		cout << RED << "Error: Invalid Multipart end count" << EOC << std::endl;
		return (true);
	}
	while (std::getline(ss, line)){
		if (lineInd == 0 && line != boundaryStart){

			rq.setStatusCode(STATUS_400);
			cout << RED << "Error: Invalid Multipart start" << EOC << std::endl;
			return (true);
		}
		if (line == boundaryEnd){
			endInd = lineInd;
		}
		++lineInd;
	}
	if (lineInd != endInd + 1){
		rq.setStatusCode(STATUS_400);
		cout << RED << "Error: Invalid Multipart end" << EOC << std::endl;
		return (true);
	}
	boundaryStart += "\n";
	vecStr	multiparts = ft_split_by_string(body, boundaryStart );
	if (multiparts.size() == 0){
		rq.setStatusCode(STATUS_500);
		cout << RED << "Error: Couldn't save multipart body" << EOC << std::endl;
		return (true);
	}

	size_t	pos = 0;
	if ( (pos = multiparts[multiparts.size() - 1].rfind(boundaryEnd += "\n")) == string::npos ){
		rq.setStatusCode(STATUS_400);
		cout << RED << "Error: Couldn't trim multipart end" << EOC << std::endl;
		return (true);
	}
	else{
		multiparts[multiparts.size() - 1] = multiparts[multiparts.size() - 1].substr(0, pos);
	}

	string	allMultiparts;
	size_t index = 0;
	for (vecStr::iterator it = multiparts.begin(); it != multiparts.end(); ++it){
		if (it->size())
			allMultiparts += *it;
		++index;
	}

	rq.setBody(allMultiparts);
	return (false);
}

bool read_multipart(int &client, URI &rq){
	char request[MAXLINE];
	string	boundaryStart = "--" + rq.getBoundary(), boundaryEnd = "--" + rq.getBoundary() + "--";
	ssize_t	readed;


	if (rq.getBody().find(boundaryEnd) == string::npos) {
		memset(request, 0, MAXLINE);
		readed = recv(client, request, sizeof(request), 0);
		if (readed <= 0){
			if (readed == 0){
				cerr << CYA << "Connection closed by client" << EOC << std::endl;	
			}
			else {	
				cerr << RED << "Error: Couldn't read multipart from that client fd" << EOC << std::endl;
			}
			return (true);
		}
		string	tmp(request, readed);
		if (tmp.size() > 0){
			rq.setRequest(rq.getRequest() += tmp);
			rq.setBody(rq.getBody() += tmp);
		}
		tmp.clear();
	}
	if (rq.getBody().find(boundaryEnd) != string::npos) {
		if (invalid_multipart(rq, boundaryStart += "\r", boundaryEnd += "\r")){
			cerr << RED << "Error: Invalid multipart" << EOC << std::endl;
			return (true);
		}
		rq.setContentLength(rq.getBody().size());
		rq.setIsMultipart(false);
	}
	return (false);
}
