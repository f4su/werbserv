#include	"../inc/request_parser.hpp"

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
	char	mth;
	line[0] == "GET" ? mth = 'g' :
		line[0] == "POST" ? mth = 'p' : 
		line[0] == "DELETE" ? mth = 'd' :
		mth = 'u';
	if (mth == 'u'){
		cout << RED << "Request error: Invalid HTTP protocol version" << EOC << std::endl;
		rq.setStatusCode(STATUS_405);
		return (true);
	}

	//Checking URI tokens
	rq.setMethod(mth);
	if (line[1].size() > MAX_URI_SIZE){
		rq.setStatusCode(STATUS_414);
		return (true);
	}
	if (invalid_uri(line[1], rq)){
		rq.setStatusCode(STATUS_400);
		return (true);
	}
	return (false);
}

bool	invalid_uri(const string &token, URI &rq){
	if (invalid_chars(token)){
		cout << RED << std::endl << "\tRequest error: Invalid URI (not properly encoded)" << EOC << std::endl;
		return (true);
	}

	cout << "URI to check is: [" << token << "]" << std::endl;
	//We need to determine the URI form: origin (o), absolute (a), authority (y) or asterisk
	char	form = 'u'; //u = undefined
	determine_uri_form(token, &form);
	if (form == 'u'){
		cout << RED << std::endl << "\tRequest error: Invalid URI form" << EOC << std::endl;
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
			return (true);
		}
	}

	//Origin Form doesn't have specific parsing since it's in included in Authority and Absolute forms
	if (invalid_values(token, rq, path_start, params_start, frag_start, &form)){
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

void	determine_uri_form(const string &uri, char *form){
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
		*form = 'u';
	}
	else {
		*form = 'y';
	}
}

