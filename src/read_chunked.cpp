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

/*
void	transfer_encoding(URI &rq_uri){
	bool	isChunked = false, expectContinue = false;

	mapStrVect			headers = rq_uri.getHeaders();
	vector<string>	values;
	std::cout << "Finding Headers!!!" << std::endl;
	if (headers.find(TRANSFER_ENCODING_H) != headers.end()){
		std::cout << "Key found!!!" << std::endl;
		values = headers[TRANSFER_ENCODING_H];
		if (find(values.begin(), values.end(), "chunked") != values.end()){
			isChunked = true;
			std::cout << "Is Chunked !!!" << std::endl;
		}
	}
	if (headers.find(EXPECT_H) != headers.end()){
		std::cout << "Expect Key found!!!" << std::endl;
		values = headers[EXPECT_H];
		if (find(values.begin(), values.end(), "100-continue") != values.end()){
			expectContinue = true;
			std::cout << "Is Expect: 100-continue!!!" << std::endl;
		}
		else {
			if (rq_uri.getStatusCode().size() == 0){
				rq_uri.setStatusCode(STATUS_417);
			}
		}
	}
		std::cout << "Finded Headers!!!" << std::endl;
	rq_uri.setIsChunked(isChunked);
	rq_uri.setExpectContinue(expectContinue);
}*/

bool invalid_chunk(URI &rq){
	size_t	size = rq.getChunkSize();
	size_t	lastLoopIndex = 0;
	string	readed = rq.getBody();

	cout << "Full Body is ->[" << readed << "]" << std::endl;
	size = 1;
	while(size != 0){
		size_t newLine = readed.find(CRLF, size);
		if (newLine == string::npos || newLine == 0){
			rq.setStatusCode(STATUS_400);
			cout << RED << "Error: Hex Chunk size not found" << EOC << std::endl;
			return (true);
		}
		string sizeStr(readed.begin() + lastLoopIndex, readed.begin() + newLine);
		cout << "SizeStr is ->[" << sizeStr << "]" << std::endl;
		std::stringstream	ss(sizeStr);
		ss >> std::hex >> size;
		if (ss.fail()){
			rq.setStatusCode(STATUS_500);
			cout << RED << "Error: Couldn't convert hex to decimal" << EOC << std::endl;
			return (true);
		}
		//Delete first line
		readed.erase(readed.begin() + lastLoopIndex , readed.begin() + newLine + 2);
		cout << CYA << "ErasedStr" << EOC << " is ->[\n" << displayHiddenChars(readed) << "]" << std::endl;
		cout << CYA << "Readed Size" << EOC << " is ->[" << readed.size() << "] and ChunkSize is [" << size << "]" << std::endl;
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
		cout << CYA << "ErasedStr After" << EOC << " is ->[\n" << displayHiddenChars(readed) << "]" << std::endl;
		rq.setBody(readed);
	}
	return (false);
}

bool read_chunked(int &client, URI &rq){
	char request[MAXLINE];
	ssize_t	readed;


	cout << CYA << "Enters Chunked" << EOC << std::endl;
	if (rq.getBody().find(CRLF0x2) == string::npos) {
		cout << CYA << "Chunked Body" << EOC << std::endl;
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
			cerr << RED << "Error: Invalid chunking" << EOC << std::endl;
			return (true);
		}
		rq.setIsChunked(false);
	}
	cerr << RED << "Ends chunked" << EOC << std::endl;	
	return (false);
}
