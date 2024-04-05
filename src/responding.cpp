#include	"../inc/responding.hpp"
#include	"../inc/headers.hpp"
#include	"../inc/launch_servers.hpp"
#include	"../inc/Utils.hpp"
#include	"../inc/Cgi.hpp"
#include	"../inc/Mime.hpp"
#include	"../inc/Response.hpp"
#include <vector>
#include <string>
#include <numeric>

#define	CRLF					"\r\n"
#define	CRLFx2				"\r\n\r\n"
// #define	DEFAULT_INDEX	"index.html"


bool	responding_when_error(int &client, Server &server, URI &rq){
	string	response("HTTP/1.1 ");
	string	parseErrs("400 404 405 406 409 411 413 414 415 417 500 501 502 505 507");
	string	status(rq.getStatusCode());

	cout << "\tStatus is->[" << rq.getStatusCode() << "]" << std::endl;
	status = status.size() > 0 ? status.substr(0, 3) : status;
	if (status.size() > 0  && parseErrs.find(status) != string::npos){
		response += rq.getStatusCode();
		response += CRLF;
		fill_4xx_5xx_response(response, rq, server);
		cout << "\tGoing to response Error->" << response << std::endl;
		if (send(client, response.c_str(), response.size(), 0) == -1){
			cerr << RED << "Error: Couldn't send response for client " << client << EOC << std::endl;
		}
		rq.setCloseConnection(true);
		return (true);
	}
	return (false);
}

int resolveRedirIndex(Server &server, URI &rq)
{
	vector<Route>		routes = server.getRoutes();
	size_t				ind = 0;
	int					pathMatch = -1;

	//Redirect??

	cout << "Status después de Jose is->[" << rq.getStatusCode() << "]" << std::endl;
	cout << MAG << "Path antess de RESOLVE is->[" << rq.getPath() << "]" << EOC << std::endl;
	for (vector<Route>::iterator it = routes.begin(); it != routes.end(); ++it){
		if (it->getPath() == rq.getPath()){
			cout << MAG << "it->Path is->[" << it->getPath() << "]" << EOC << std::endl;
			pathMatch = ind;
			//Redirect
			if (it->getRedirect().size()){
				rq.setPath(it->getRedirect());	//Setear status code a 301??
			}

			//Root
			else if (it->getRoot().size()){
				rq.setPath(it->getRoot() + rq.getPath());
			}
			else if (server.getRoot().size()){
				rq.setPath(server.getRoot() + rq.getPath());
			}
/* 			cout << MAG << "ALLOW LISTING IS-->" << it->getAllowListing() << "]" << EOC << std::endl;
			//AllowListing
			if (it->getAllowListingSet() == false){
				cout << MAG << "SETTING PAPA-->" << server.getAllowListing() << "]" << EOC << std::endl;
				it->setAllowListing(server.getAllowListing());
			} */

			//Index
			if (it->getIndex().size()){
				rq.setPath(rq.getPath() + it->getIndex()[0]);
			}
			else{
				if (server.getIndex().size()){
					rq.setPath( rq.getPath() + server.getIndex()[0]);
				}
			}
			return (pathMatch);
		}
		++ind;
	}

	if (pathMatch == -1){
		rq.setStatusCode(STATUS_404);
	}
	cout << MAG << "Path después de RESOLVE is->[" << rq.getPath() << "]" << EOC << std::endl;
	return (pathMatch);

}

void respond_connection(int &client, Server &server, URI &rq){
	check_body_size(server, rq);
	size_t	routeInd;
	if (rq.getStatusCode().size() == 0){
		routeInd = resolveRedirIndex(server, rq);
	}
	if (responding_when_error(client, server, rq) && rq.getPath() != "/make"){
		return ;
	}
	/* if (rq.getPath() == "/"){
		rq.setPath(rq.getPath() + server.getRoot() + "/" + server.getIndex()[0]);
		std::cerr << MAG << "PATHHhHHHHHHHHHHHHHHHHH--->" << rq.getPath() << EOC << std:endl;
	} */

	rq.setStatusCode(STATUS_200);
	cout << "Status Antes de Jose is->[" << rq.getStatusCode() << "]" << std::endl;
	Response	response(rq);

	response.handleResponse(server, server.getRoutes()[routeInd]);
	std::string res = response.getResponse();
	cout << "Status después de Jose is->[" << rq.getStatusCode() << "]" << std::endl;

	cerr << CYA << "\nRESPONSE>:\n" << displayHiddenChars(res) << "\n]" << EOC << std::endl;

	if ( send(client, res.c_str(), res.size(), 0) == -1){
		cerr << RED << "Error: Couldn't send response for client " << client << EOC << std::endl;
	}

	cout << RED << "Response sent " << client << " !!" << EOC << std::endl;
	rq.setCloseConnection(true);
}




void	add_error_status_code_msg(string &response, URI &rq){
	string	code = rq.getStatusCode();

	if (code.size() < 4 || (code.front() != '4' && code.front() != '5')){
		return ;
	}
	string	msg = code.substr(4, code.size());
	msg += CRLF;
	cout << CYA << "MSG +++++++>>>[" << msg << "]" << EOC << std::endl;
	response += "Content-Type: text/plain\r\n";
	response += "Content-Lenght: ";
	string	strSize = std::to_string(msg.size());
	response += strSize;
	response += CRLFx2;
	response += msg;
	cout << CYA << "RESPONSE+++++++>>>[" << response << "]" << EOC << std::endl;
}

void	add_error_page_response(string &response, string &filePath, URI &rq){
	cout << CYA << "PATH+++++++>>>[" << filePath << "]" << EOC << std::endl;
	std::ifstream file(filePath);

	if (file.is_open())
	{
		Mime	mime;
	
		response += "Content-Type: ";
		response += "text/html; charset=UTF-8";
		//response += mime[Cgi::getFileExt(filePath)];		NOT WORKINGGGG
		response += CRLF;
		std::stringstream buffer;
		string line;
		string body;
		while (std::getline(file, line))
				buffer << line << std::endl;
		body = buffer.str();
		file.close();

		response += "Content-Lenght: ";
		response += body.size();
		response += CRLFx2;
		response += body;
		cout << CYA << "RESPONSE+++++++>>>[" << response << "]" << EOC << std::endl;
	}
	else{
		add_error_status_code_msg(response, rq);
	}
}

void	fill_4xx_5xx_response(string &response, URI &rq, Server &server){
	string	code = rq.getStatusCode();

	cout << CYA << "STATUS-CODE+++++++>>>[" << code << "]" << EOC << std::endl;
	if (code.size() == 0 || (code.front() != '4' && code.front() != '5')){
		return ;
	}
	string						strNb = code.substr(0, 3);
	int 							nb		= 0;
	cout << CYA << "CODE+++++++>>>[" << strNb << "]" << EOC << std::endl;
	std::stringstream	ss(strNb);
	ss >> nb;
	if (ss.fail() || nb < 400 || nb > 599){
		return (add_error_status_code_msg(response, rq));
	}

	cout << CYA << "RQ_PATH+++++++>>>[" << rq.getPath() << "]" << EOC << std::endl;
	vector<Route>	routes = server.getRoutes();
	for (vector<Route>::iterator it = routes.begin(); it != routes.end(); ++it){
		if (rq.getPath() != it->getPath()) continue ;
		if (it->getErrorPages().find(nb) != it->getErrorPages().end()){
			cout << CYA << "ERR PAGE IN PATH++++++" << EOC << std::endl;
			return ( add_error_page_response(response, (it->getErrorPages())[nb], rq) );
		}
	}


	mapIntStr	errPages = server.getErrorPages();
	for (mapIntStr::iterator it = errPages.begin(); it != errPages.end(); ++it){
		cout << CYA << "KEYY++:" << it->first << EOC << std::endl;
	}
	if (errPages.find(nb)  != errPages.end()){
		cout << CYA << "ERR PAGE IN SERVER++++++SIZE:" << server.getErrorPages().size() << EOC << std::endl;
		return ( add_error_page_response(response, (server.getErrorPages())[nb], rq) );
	}
	return (add_error_status_code_msg(response, rq));
}

void check_body_size(Server &server, URI &rq){
	size_t			bodySize			= rq.getBody().size();
	size_t			max 					= server.getClientMaxBodySize();
	size_t			contentLength	=	0;
	bool				length_h			= false;
	mapStrVect	hdrs					= rq.getHeaders();

	if (rq.getStatusCode().size()){
		return ;
	}
	if (hdrs.find(CONTENT_LENGTH_H) != hdrs.end()){
		length_h = true;
		string val = (hdrs[CONTENT_LENGTH_H])[0];
		std::stringstream	ss(val);
		ss >> contentLength;
		if (ss.fail()){
			cout << RED << "Error: Invalid Content-Length value(400)" << EOC << std::endl; 
			rq.setStatusCode(STATUS_400);
			return ;
		}
	}

	if (bodySize > 0){
		if (max != 0 && bodySize > max){
			rq.setStatusCode(STATUS_413);
			cout << RED << "Error: Body too long (413 Entity too long)" << EOC << std::endl; 
		}
		else if (length_h == true && contentLength != bodySize){
			rq.setStatusCode(STATUS_400);
			cout << RED << "Error: Content-Length  mismatches body size (400)" << EOC << std::endl; 
		}
		else if (length_h == false){
			rq.setStatusCode(STATUS_411);
			cout << RED << "Error: Body found, but Content-Length is missing (411)" << EOC << std::endl; 
		}
	}
	else {
		if (length_h == true && contentLength > 0){
			rq.setStatusCode(STATUS_400);
			cout << RED << "Error: Content-Length set, but no body size (400)" << EOC << std::endl; 
		}
	}
}
