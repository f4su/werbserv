
#include <vector>
#include <string>
#include <sstream>
#include	"../inc/request_parser.hpp"

#define	MAX_REQUEST_SIZE 
#define	REQUEST_MIN_LINES 2

using	std::cout;
using	std::vector;


bool	invalid_path_syntax(string &path){
	//We will implement a basic path checker since this step is done only if open() has failed.

	for (char c : path){
		if ((c < 33) ||									//	Whitespaces
				(c == 34) ||								//	"
				(c == 60) || (c == 62) ||		//	< >
				(c > 90 && c < 95) ||				//	[ \ ] ^
				(c == 96) ||								//	`
				(c > 122 && c < 126) ||			//	{ | }
				(c > 126)){									//	del
			return (true)
		}
	}

	return (false);
}

void	determine_uri_form(const string &uri, char *form){
	//Asterisk	Form must begin with * and cannot be performed with GET, POST or DELETE methods
	//Origin 		Form begins with /
	//Absolute	Form must have ://
	//Authority	Form will be the rest of cases

	if (uri[0] == '*'){
		return ;
	}
	else if (uri[0] == '/'){
		*form = 'o';
	}
	else if (uri.find("://") != string::npos){
		*form = 'a';
	}
	else {
		*form = 'y';
	}
}

bool	invalid_uri(const string &token, URI &rq_uri){
	//First, we need to determine the URI form: origin (o), absolute (a), authority (y) or asterisk
	char	form = 'u'; //u = undefined

	determine_uri_form(token, &form);
	cout << "The URI form is: [" << form << "]\n";
	if (form == 'u'){
		cout << "\n\tRequest error: Invalid URI form\n";
		return (true);
	}

	//Parsing Absolute Form
	if (form == 'a'){
		//Shortest absolute form: "http:///", 8 chars
		//Scheme must be http (we aren't going to implement HTTP) and end with : before a backslash
		if (token.size() < 8 || token.compare(0, 6, "http://") != 0){
			cout << "\n\tRequest error: Invalid URI scheme\n";
			return (true);
		}
		rq_uri.setScheme("http");

		size_t	path_start = token.find("/", 7);
		size_t	params_start = token.find_first_of("?", 7);
		size_t	frag_start = token.find("#", 7);

		//Authority (optinal) must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
		if (path_start !=  string::npos && path_start > 7){
			rq_uri.setAuthority(token.substr(7, path_start - 7));
			//check auth syntax & port
			int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0666);
			if (fd == -1) {
				if (invalid_path_syntax(rq_uri.getPath())){
					cout << "\n\tRequest error: Invalid URI authority\n";
					return (true);
				}
			}
			else{
				close(fd);
			}
		}

		//Path must begin with a / and end with ?, # or the end of the URI. It's not applicable in Authority form
		if (path_start != string::npos){
			if (params_start != string::npos){
				rq_uri.setPath(token.substr(path_start, params_start - path_start));
			}
			else if (frag_start != string::npos){
				rq_uri.setPath(token.substr(path_start, frag_start - path_start));
			}
			else{
				rq_uri.setPath(token.substr(path_start, token.size() - path_start));
			}
			//check path syntax
			if (invalid_path_syntax(rq_uri.getPath())){
				cout << "\n\tRequest error: Invalid URI path syntax\n";
				return (true);
			}
		}
		else {
			cout << "\n\tRequest error: URI without path\n";
			return (true);
		}


		//Params/Query (optional) must begin with a ? and end with # or the end of the URI. It's only applicable in absolute form
		if (params_start != string::npos){
			if (frag_start != string::npos){
				rq_uri.setParams(token.substr(params_start, frag_start - params_start));
			}
			else{
				rq_uri.setParams(token.substr(params_start, token.size() - params_start));
			}
			//check params syntax
			if (invalid_query_syntax(rq_uri.getParams()))
			{
				cout << "\n\tRequest error: Invalid URI params/query\n";
				return (true);
			}
		}

		//Fragment (optional) must begin with a # and end with the end of the URII. It's only applicable in absolute form
		if (frag_start != string::npos){
			rq_uri.setFragment(token.substr(frag_start, token.size() - frag_start));
			cout << "\n\tRequest error: Invalid URI fragment\n";
			return (true);
		}
	}

	//Parsing Origin Form
	else if (form == 'o'){
		//Path must begin with a / and end with ?, # or the end of the URI. It's not applicable in Authority form

	}

	//Parsing Authority Form
	else {
		//Authority must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
		if (){
			cout << "\n\tRequest error: Invalid URI authority\n";
			return (true);
		}

	}
	

	
	

	return (false);
}


bool invalid_start_line(vector<string> const &start_line)
{
	//Checking total tokens
	if (start_line.size() != 3){
		cout << "Request error: Invalid start-line tokens\n";
		return (true);
	}

	//Checking method token
	char	method = 'u';
	if (start_line[0].compare("GET") == 0){
		method = 'g';
	}
	else if (start_line[0].compare("POST") == 0){
		method = 'p';
	}
	else if (start_line[0].compare("DELETE") == 0){
		method = 'd';
	}
	else
	{
		cout << "Request error: Invalid HTTP method\n";
		return (true);
	}

	//Checking protocol token
	if (start_line[2].compare("HTTP/1.1") != 0){
		cout << "Request error: Invalid HTTP protocol version\n";
		return (true);
	}

	//Checking URI tokens
	URI	rq_uri;
	if (invalid_uri(start_line[1], rq_uri)){
		cout << "Request error: Invalid URI\n";
		return (true);
	}
	return (false);
}

bool invalid_header(char const *header)
{

	return (false);
}

/*
size_t	find_CRLF(const char	*request, bool is_start_line){
	size_t		end = 0;
	char		*	crlf = std::strstr(request, "\r\n");

	//	start-line doesn't need to have CRLF nor LF
	//	if start-line doesn't have CRLF or LF, total lenght is returned
	if (crlf == nullptr && is_start_line){
		crlf = std::strchr(request, '\n');
		if (crlf == nullptr){
			end = std::strlen(request);
			return (end);
		}
	}

	//	if CRLF isn't present, 0 is returned
	if (crlf == nullptr){
		cout << "Request error: CRLF not found\n";
		return (end);
	}

	size_t end = static_cast<size_t>(crlf - request);
	
	return (end);
}
*/

void	tokenizer(string &raw_request, vector<vector<string> >	&tokens)
{
	std::stringstream 	r_rq(raw_request);
	string							line;

	while (std::getline(r_rq, line))
	{
		vector<string>			words;
		string							word;
		std::stringstream		line_stream(line);

		while (line_stream >> word)
		{
			words.push_back(word);
		}
		tokens.push_back(words);
	}
}

bool invalid_request(const char *request)
{
	if (request == nullptr){
		return (1);
	}

	string	raw_request(request);
	cout << "\nValidating following request of size [" << raw_request.size() << "]:\n[" << raw_request << "]\n\n";

	//Creating a container for lines and words
	vector<vector<string> >	tokens;
	tokenizer(raw_request, tokens);

	for (size_t i = 0; i < tokens.size(); ++i){
		cout << "Line [" << i << "]:\n";
		for (size_t j = 0; j < tokens[i].size(); ++j){
			cout << "\tWord [" << j << "]:" << tokens[i][j] << "\n";
		}
	}

	if (tokens.size() < REQUEST_MIN_LINES){
		return (1);
	}

	if (invalid_start_line(tokens[0])){
		cout << "Request error: Invalid start line\n";
		return (true);
	}

	if (invalid_header(request)){
		cout << "Request error: Invalid header\n";
		return (true);
	}

	return (false);
}



int	main(int argc, char **argv)
{
	if (invalid_request(" Holaa me    llamo  Juan\nY soy   el  conquistador \ndel fin   del   mundo "))
	{
		return (1);
	}
	return (0);
}
