#ifndef	A_HPP
# define	A_HPP

#include "request_parser.hpp"
#include	"status_codes.hpp"

using std::cout;
using std::cerr;

//	../src/responding.cpp
void respond_connection(int &client, Server &server, URI &rq);
void check_body_size(Server &server, URI &rq);
void	fill_4xx_5xx_response(string &response, URI &rq, Server &server);
void	add_error_status_code_msg(string &response, URI &rq);
void	add_error_page_response(string &response, string &filePath, URI &rq);


#endif

