
#include <vector>
#include <string>
#include <sstream>
#include	"../inc/request_parser.hpp"
#include	"../inc/Utils.hpp"

#define	MAX_REQUEST_SIZE 
#define	REQUEST_MIN_LINES 2

using	std::cout;
using	std::vector;


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


bool	invalid_authority(const string &auth, URI &rq_uri){
	//Authority is composed by:	userinfo	@	host	: port
	//We are not going to deal with userinfo
	size_t	host_start = auth.find('@');
	size_t	host_end = auth.find_last_of(':');

	if (host_start == auth.size() || host_end == auth.size()){
		return (true);
	}

	if (host_start == string::npos){
		host_start = 0;
	}
	else {
		host_start += 1;
	}

	if (host_end == string::npos){
		host_end = auth.size();
	}
	else {
		string::const_iterator it;
		for (it = auth.begin() + host_end + 1; it != auth.end(); ++it){
			if (*it < 48 || *it > 57){
				return (true);
			}
		}
		rq_uri.setPort(auth.substr(host_end + 1, auth.size()));
	}
	rq_uri.setHost(auth.substr(host_start, host_end));
	return (false);
}

bool	invalid_query_syntax(const string &query, URI &rq_uri){
	//We will only accept & as separator for keys and values
	if (query.find("&&") != string::npos || query.find("==") != string::npos){
		return (true);
	}
	std::vector<std::string>	params = ft_split(query, "&");
	std::vector<std::string>::const_iterator it;
	std::map<string, string>	prms;
	for (it = params.begin(); it != params.end(); ++it){
		std::vector<std::string>	pairs = ft_split(*it, "=");
		if (pairs.size() != 2){
			return (true);
		}
		prms.insert(std::make_pair(pairs[0], pairs[1]));
	}
	rq_uri.setParams(prms);
	return (false);
}

bool	invalid_values(const string &token, URI &rq_uri, size_t p_start, size_t pr_start, size_t f_start, char *form){

	//Authority (optinal) must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
	if ((*form == 'a' || *form == 'y') && p_start !=  string::npos){
		//Shortest authority possible:		http://a/			//a/
		//		 i													012345678			0123
		int	lenght = 0;
		*form == 'a' ? lenght = 7 : *form == 'y' ? lenght = 2 : lenght = 0;
		if (lenght > 0){
			rq_uri.setAuthority(token.substr(lenght, p_start - lenght));
		}
		//check auth syntax & port
		if (lenght == 0 || invalid_authority(rq_uri.getAuthority(), rq_uri)){
			cout << "\n\tRequest error: Invalid URI authority syntax\n";
			return (true);
		}
	}

	//Path must begin with a / and end with ?, # or the end of the URI. It's not applicable in Authority form
	if (p_start != string::npos){
		if (pr_start != string::npos){
			rq_uri.setPath(token.substr(p_start, pr_start - p_start));
		}
		else if (f_start != string::npos){
			rq_uri.setPath(token.substr(p_start, f_start - p_start));
		}
		else{
			rq_uri.setPath(token.substr(p_start, token.size() - p_start));
		}
	}
	else {
		cout << "\n\tRequest error: URI without path\n";
		return (true);
	}

	//Params/Query (optional) must begin with a ? and end with # or the end of the URI. It's only applicable in absolute form
	if (pr_start != string::npos){
		if (f_start != string::npos){
			rq_uri.setQuery(token.substr(pr_start, f_start - pr_start - 1));
		}
		else{
			rq_uri.setQuery(token.substr(p_start, token.size() - pr_start - 1));
		}
		//check params syntax
		if (invalid_query_syntax(rq_uri.getQuery(), rq_uri))
		{
			cout << "\n\tRequest error: Invalid URI params/query\n";
			return (true);
		}
	}

	//Fragment (optional) must begin with a # and end with the end of the URI. It's only applicable in absolute form
	if (f_start != string::npos){
		rq_uri.setFragment(token.substr(f_start, token.size() - f_start - 1));
	}
	return (false);
}

void	determine_uri_form(const string &uri, char *form){
	//Asterisk	Form must begin with * and cannot be performed with GET, POST or DELETE methods
	//Origin 		Form begins with /
	//Authority	Form must start with //
	//Absolute	Form must have http://	(we are going to implement only http scheme)

	if (uri.find("http://") == 0){
		*form = 'a';
	}
	else if (uri.find("//") == 0){
		*form = 'y';
	}
	else if (uri[0] == '/'){
		*form = 'o';
	}
	else {
		*form = 'u';
	}
}

bool	invalid_uri(const string &token, URI &rq_uri){
	if (invalid_chars(token)){
		cout << "\n\tRequest error: Invalid URI (not properly encoded)\n";
		return (true);
	}

	//We need to determine the URI form: origin (o), absolute (a), authority (y) or asterisk
	char	form = 'u'; //u = undefined
	determine_uri_form(token, &form);
	cout << "The URI form is: [" << form << "]\n";
	if (form == 'u'){
		cout << "\n\tRequest error: Invalid URI form\n";
		return (true);
	}

	size_t	path_start = token.find("/", 0);
	size_t	params_start = token.find("?", 0);
	size_t	frag_start = token.find("#", 0);

	//Parsing Absolute Form
	if (form == 'a'){
		//Shortest absolute form: "http:///", 8 chars
		//Scheme must be http (we aren't going to implement HTTP) and end with : before a backslash
		if (token.size() < 8 || token.compare(0, 6, "http://") != 0){
			cout << "\n\tRequest error: Invalid URI scheme\n";
			return (true);
		}
		rq_uri.setScheme("http");

		path_start = token.find("/", 8);
		params_start = token.find("?", 8);
		frag_start = token.find("#", 8);

		/*if (invalid_absolute_form(token, rq_uri)){
			return (true);
		}*/
	}

	//Parsing Authority Form
	else if (form == 'y'){
		//Shortest absolute form: "//a/", 4 chars
		//Authority must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
		if (token.size() < 4 || token.compare(0, 6, "http://") != 0){
			cout << "\n\tRequest error: Invalid URI scheme\n";
			return (true);
		}
		path_start = token.find("/", 4);
		params_start = token.find("?", 4);
		frag_start = token.find("#", 4);
	}

	//Origin Form doesn't have specific parsing since it's in included in Authority and Absolute forms
	if (invalid_values(token, rq_uri, path_start, params_start, frag_start, &form)){
			return (true);
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

//////////////////////////////////////////////////////////////////////////////////
std::string methods[] = {
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "HEAD",
    "CONNECT",
    "OPTIONS",
    "TRACE",
    "PATCH",
};

std::string URI::getMethod() const {
    return method;
}

std::string URI::getUri() const {
    return uri;
}

std::string URI::getVersion() const {
    return version;
}

std::map<std::string, std::string> URI::getHeaders() const {
    return headers;
}

std::string URI::getHost() const {
    return host;
}

int URI::getPort2() const {
    return port2;
}

std::string URI::getRawBody() const {
    return rawBody;
}
////////////////////////////////////////////////////