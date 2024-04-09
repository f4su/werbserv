#include	"../inc/request_parser.hpp"
#include	"../inc/headers.hpp"

#define	MAX_URI_SIZE	2000		//Based on Chrome

using	std::cout;

bool invalid_start_line(vector<string> const &line, URI &rq){
	//Checking total tokens
	if (line.size() != 3){
		cout << RED << "Request Error: Invalid start-line tokens" << EOC << std::endl;
		rq.setStatusCode(STATUS_400);
		return (true);
	}

	//Checking protocol token
	if (line[2].compare("HTTP/1.1") != 0){
		cout << RED << "Request error: Invalid HTTP protocol version" << EOC << std::endl;
		rq.setStatusCode(STATUS_505);
		return (true);
	}

	//Checking method token
	if (invalid_method(line[0], rq)){
		cout << RED << std::endl << "\tRequest error: Invalid method" << EOC << std::endl;
		return (true);
	}

	//Checking URI tokens
	if (line[1].size() > MAX_URI_SIZE){
		rq.setStatusCode(STATUS_414);
		cout << RED << std::endl << "\tRequest error: URI too long" << EOC << std::endl;
		return (true);
	}
	if (invalid_uri(line[1], rq)){
		cout << RED << std::endl << "\tRequest error: Invalid URI" << EOC << std::endl;

		return (true);
	}
	return (false);
}

bool	invalid_method(string const &method, URI &rq){
	string	all_mth(ALL_METHODS);
	char	mth;

	mth = method == "GET" ? 'g' :
		method == "POST" ? 'p' : 
		method == "DELETE" ? 'd' :
		'u';
	if (mth == 'u'){
		if (all_mth.find(method) == string::npos){
			cout << RED << "Request error: HTTP method doesn't exist (400)" << EOC << std::endl;
			rq.setStatusCode(STATUS_400);
			return (true);
		}
		cout << RED << "Request error: Method not implemented (501)" << EOC << std::endl;
		rq.setStatusCode(STATUS_501);
		return (true);
	}
	rq.setMethod(mth);
	return (false);
}

bool	invalid_uri(const string &token, URI &rq){
	if (invalid_chars(token)){
		cout << RED << std::endl << "\tRequest error: Invalid URI (not properly encoded)" << EOC << std::endl;
		rq.setStatusCode(STATUS_400);
		return (true);
	}
	//We need to determine the URI form: origin (o), absolute (a), authority (y) or asterisk
	char	form = 'u'; //u = undefined
	if (determine_uri_form(token, &form)){
		rq.setStatusCode(STATUS_400);
		return (true);
	}

	size_t	path_start = token.find("/", 0);
	size_t	params_start = token.find("?", 0);
	size_t	frag_start = token.find("#", 0);

	//Parsing Absolute Form
	if (form == 'a'){
		//Shortest absolute form: "http:///", 8 chars
		//Scheme must be http (we aren't going to implement HTTP) and end with : before a backslash
		if (token.size() < 8 || token.compare(0, 7, "http://") != 0){
			cout << RED << std::endl << "\tRequest error: Invalid URI scheme" << EOC << std::endl;
			rq.setStatusCode(STATUS_400);
			return (true);
		}
		rq.setScheme("http");

		path_start = token.find("/", 8);
		params_start = token.find("?", 8);
		frag_start = token.find("#", 8);

	}

	//Parsing Authority Form
	else if (form == 'y'){
		//Shortest absolute form: "a", 1 char
		//Authority must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
		if (token.size() < 1 || path_start != string::npos || params_start != string::npos || frag_start != string::npos){
			cout << RED << std::endl << "\tRequest error: Invalid URI scheme(authority form)" << EOC << std::endl;
			rq.setStatusCode(STATUS_400);
			return (true);
		}
	}

	//Origin Form doesn't have specific parsing since it's in included in Authority and Absolute forms
	if (invalid_values(token, rq, path_start, params_start, frag_start, &form)){
		rq.setStatusCode(STATUS_400);
		return (true);
	}

	return (false);
}

bool	invalid_chars(const string &path){
	string::const_iterator	it;
	for (it = path.begin(); it != path.end(); ++it){
		if ((*it < 33) ||									//	Whitespaces
				(*it == 34) ||								//	"
				(*it == 60) || (*it == 62) ||	//	< >
				(*it > 90 && *it < 95) ||			//	[ \ ] ^
				(*it == 96) ||								//	`
				(*it > 122 && *it < 126) ||		//	{ | }
				(*it > 126)){									//	del
			return (true);
		}
	}
	return (false);
}

bool	determine_uri_form(const string &uri, char *form){
	//Asterisk	Form must begin with * and cannot be performed with GET, POST or DELETE methods
	//Origin 		Form begins with /
	//Authority	Form must start with the IP/Domain Name
	//Absolute	Form must have http://	(we are going to implement only http scheme)

	if (uri.find("http://") == 0){
		*form = 'a';
	}
	else if (uri[0] == '/'){
		*form = 'o';
	}
	else if (uri[0] == '*'){
		cout << RED << std::endl << "\tRequest error: Invalid URI form" << EOC << std::endl;
		return (true);
	}
	else {
		*form = 'y';
	}
	return (false);
}

bool	invalid_method_in_route(URI &rq, Route &route){
	char						mth = rq.getMethod();
	string					method;
	vector<string>	route_mths = route.getMethods();

	method = mth == 'g' ? "GET" : mth == 'p' ? "POST" : mth == 'd' ? "DELETE" : "";

	if (route_mths.size() == 0 && method.size())
		return false;
	if (std::find(route_mths.begin(), route_mths.end(), method) == route_mths.end()){
		rq.setStatusCode(STATUS_501);
		cout << RED << std::endl << "\tRequest error: Method not implemented on path (501)" << EOC << std::endl;
		return (true);
	}

	return (false);
}
