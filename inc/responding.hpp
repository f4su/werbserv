#ifndef	A_HPP
# define	A_HPP

#include "request_parser.hpp"
#include	"status_codes.hpp"

using std::cout;
using std::cerr;

void respond_connection(int &client, Server &server, URI &rq);


#endif

