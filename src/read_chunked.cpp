#include	"../inc/request_parser.hpp"
#include	"../inc/launch_servers.hpp"
#include	"../inc/headers.hpp"
#include <sys/socket.h>
#include <sstream>

using	std::cout;
using	std::cerr;

#define	MAXLINE					1024 //By default, nginx sets client_header_buffer_size to 1 kilobyte (1024 bytes)
#define	CRLF						"\r\n"
#define	CRLF0x2					"0\r\n\r\n"

bool invalid_chunk(URI &rq){
	size_t	size = rq.getChunkSize();
	size_t	lastLoopIndex = 0;
	string	readed = rq.getBody();

	size = 1;
	while(size != 0){
		size_t newLine = readed.find(CRLF, size);
		if (newLine == string::npos || newLine == 0){
			rq.setStatusCode(STATUS_400);
			cout << RED << "Error: Hex Chunk size not found" << EOC << std::endl;
			return (true);
		}
		string sizeStr(readed.begin() + lastLoopIndex, readed.begin() + newLine);
		std::stringstream	ss(sizeStr);
		ss >> std::hex >> size;
		if (ss.fail()){
			rq.setStatusCode(STATUS_500);
			cout << RED << "Error: Couldn't convert hex to decimal" << EOC << std::endl;
			return (true);
		}
		//Delete first line
		readed.erase(readed.begin() + lastLoopIndex , readed.begin() + newLine + 2);
		//Delete end CRLF
		if (size > 0){
			if (readed.size() < size || readed[size] != '\r' || readed[size + 1] != '\n'){
				rq.setStatusCode(STATUS_400);
				cout << RED << "Error: at the end of the chunk" << EOC << std::endl;
				return (true);
			}
			readed.erase(readed.begin() + size, readed.begin() + size + 2);
		}
		else {
			if (readed.size() != lastLoopIndex + 2){
				rq.setStatusCode(STATUS_400);
				cout << RED << "Error: at the end of the chunked request" << EOC << std::endl;
				return (true);
			}
			readed.erase(readed.begin() + lastLoopIndex, readed.end());
		}
		lastLoopIndex += size;
		rq.setBody(readed);
	}
	return (false);
}

bool read_chunked(int &client, URI &rq){
	char request[MAXLINE];
	ssize_t	readed;

	if (rq.getBody().find(CRLF0x2) == string::npos) {
		memset(request, 0, MAXLINE);
		readed = recv(client, request, sizeof(request), 0);
		if (readed <= 0){
			if (readed == 0){
				cerr << CYA << "Connection closed by client" << EOC << std::endl;	
			}
			else {	
				cerr << RED << "Error: Couldn't read chunk from that client fd" << EOC << std::endl;
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
	if (rq.getBody().find(CRLF0x2) != string::npos) {
		if (invalid_chunk(rq)){
			cerr << RED << "Error: Invalid chunking" << EOC << std::endl;		//Mirar para seterar el status code
			return (true);
		}
		rq.setIsChunked(false);
	}
	return (false);
}
