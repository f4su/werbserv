#include	"../inc/responding.hpp"
#include	"../inc/headers.hpp"
#include	"../inc/Utils.hpp"

#define	CRLF			"\r\n"
#define	CRLFx2		"\r\n\r\n"


/*
void check_body_size(Server &server, URI &rq){
	size_t	bodySize = rq.getBodySize();
	size_t	max = server.getClientMaxBodySize();
	size_t	c

	if (bodySize > 0){
		if (max != 0 && bodySize > max){
			rq.setStatusCode(STATUS_413);
		}
		else if (contentLength > 0 && contentLength != ){
			rq.setStatusCode(STATUS_400);
		}
	}
	else {
		if (rq.getHeaders().find(CONTENT_LENGTH_H) != ){ //Si hay header Content Lenght, pero no hay body

		}
	}
}*/

void respond_connection(int &client, Server &server, URI &rq){

	/*
	check_body_size(server, rq);
	//string s_buff = "HTTP/1.1 ";


	//	Mirar cómo se hace una request para que sea proxy y comprobar que nuestro servidor no puede ser proxy

	cout << "\tStatus is->[" << rq.getStatusCode() << "]" << std::endl;
	string	parseErrs("400 405 406 411 413 414 415 417 501 505");
	string	status(rq.getStatusCode());
	status = status.size() > 0 ? status.substr(0, 3) : status;
	if (status.size() > 0  && parseErrs.find(status) != string::npos){
		string	response("HTTP/1.1 ");
		response += rq.getStatusCode();
		response += CRLF;
		//response += "Expectation Failed";
		response += CRLFx2;
		cout << "\tGoing to response ->" << response << std::endl;
		if (send(client, response.c_str(), response.size(), 0) == -1){
			cerr << RED << "Error: Couldn't send response for client " << client << EOC << std::endl;
		}
		rq.setCloseConnection(true);
		return ;
	}*/



	//Meter aquí Joseee la responseeeeeeeeeeeeeeeeee

/*
     ___  _______  _______  _______ 
    |   ||       ||       ||       |
    |   ||   _   ||  _____||    ___|
    |   ||  | |  || |_____ |   |___ 
 ___|   ||  |_|  ||_____  ||    ___|
|       ||       | _____| ||   |___ 
|_______||_______||_______||_______|

*/


//////////////////////////////////////////////////////////////////





	/*
	if (send(client, s_buff.c_str(), strlen(s_buff.c_str()), 0) == -1){
		//close connection?
		cerr << RED << "Error: Couldn't send response for client " << client << EOC << std::endl;
	}
*/
	

	cout << RED << "Responding client " << client << " !!" << EOC << std::endl;
	server.getSocket(); //to avoid unused
	rq.setCloseConnection(true);
}


