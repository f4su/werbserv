
#include <vector>
#include <string>
#include <sstream>
#include	"../inc/request_parser.hpp"
#include	"../inc/Utils.hpp"

#define	MAX_REQUEST_SIZE 
#define	REQUEST_MIN_LINES 3		//start-line, header, /r/n

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

bool	invalid_port(const string &port, URI &rq_uri){
	//Ports are store in 16bits. Therefore, 65,536 ports available	0 - 65535
	std::stringstream ss(port); 
	int num;

	if (port.find_first_not_of("0123456789") != string::npos){
		return (true);
	}
	if (ss >> num) { 
		if (num >= 65536){
			return (true);
		}
	}
	else {
			return (true);
	}
	rq_uri.setPort(num);
	return (false);
}

bool	invalid_authority(const string &auth, URI &rq_uri){
	//Authority is composed by:	host : port
	size_t	host_end = auth.find_last_of(':');
	size_t	ipV6in = auth.find("[");
	size_t	ipV6out = auth.find("]");
	
	if (auth.size() < 1 || auth.front() == ':' ||
			(ipV6in != string::npos && auth.size() < 3) ||
			(ipV6in != string::npos && ipV6in != 0) ||
			(ipV6out != string::npos && ipV6out != auth.size() -1) ||
			(ipV6in != string::npos && ipV6out == string::npos) ||
			(ipV6in == string::npos && ipV6out != string::npos)) {
		return (true);
	}
	if (host_end == string::npos || auth.back() == ':'){
		host_end = auth.size();
		rq_uri.setPort(80);
	}
	else {
		if (invalid_port(auth.substr(host_end + 1, auth.size()), rq_uri)){
			cout << "Invalid port\n";
			return (true);
		}
	}
	ipV6in != string::npos ? rq_uri.setHost(auth.substr(1, --host_end)) :
		rq_uri.setHost(auth.substr(0, host_end));
	return (false);
}

bool	invalid_query_syntax(const string &query, URI &rq_uri){
	//We will only accept & as separator for keys and values
	cout << "Query iss xxxx> " << query << "\n";
	if (query.size() <= 3 || query[0] != '?' || query.find("&&") != string::npos || query.find("==") != string::npos){
		return (true);
	}
	std::vector<std::string>	params;
	params = ft_split(query.substr(1, query.size()), "&");
	std::vector<std::string>::const_iterator it;
	std::map<string, string>	prms;
	for (it = params.begin(); it != params.end(); ++it){
		std::vector<std::string>	pairs = ft_split(*it, "=");
		cout << "kkkkkkkkkk ---> " << *it << "\n";
		if (pairs.size() != 2){
			return (true);
		}
		cout << "pair 0 )))> " << pairs[0] << "\npair 1 )))> " << pairs[1] << "\n";
		prms.insert(std::make_pair(pairs[0], pairs[1]));
	}
	rq_uri.setParams(prms);
	cout << rq_uri;
	return (false);
}

bool	invalid_values(const string &token, URI &rq_uri, size_t p_start, size_t pr_start, size_t f_start, char *form){

	//Authority (optinal) must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
	if ((*form == 'a' && p_start !=  string::npos) ||
			(*form == 'y')){
		//Shortest authority possible:		http://a/			a
		if (*form == 'a'){
			rq_uri.setAuthority(token.substr(7, p_start - 7));
		}
		else{
			rq_uri.setAuthority(token);
		}
		cout << "Authority iss -> " << rq_uri.getAuthority() << "\n";
		//check auth syntax & port
		if (invalid_authority(rq_uri.getAuthority(), rq_uri)){
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
	else if (*form != 'y') {
		cout << "\n\tRequest error: URI without path\n";
		return (true);
	}

	//Params/Query (optional) must begin with a ? and end with # or the end of the URI. It's only applicable in absolute form
	if (pr_start != string::npos){
		f_start != string::npos ? rq_uri.setQuery(token.substr(pr_start, f_start - pr_start - 1)) :
			rq_uri.setQuery(token.substr(pr_start, token.size() - pr_start - 1));
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

bool	invalid_uri(const string &token, URI &rq_uri){
	if (invalid_chars(token)){
		cout << "\n\tRequest error: Invalid URI (not properly encoded)\n";
		return (true);
	}

	cout << "URI to check is: [" << token << "]\n";
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
		if (token.size() < 8 || token.compare(0, 7, "http://") != 0){
			cout << "\n\tRequest error: Invalid URI scheme\n";
			return (true);
		}
		rq_uri.setScheme("http");

		path_start = token.find("/", 8);
		params_start = token.find("?", 8);
		frag_start = token.find("#", 8);

	}

	//Parsing Authority Form
	else if (form == 'y'){
		cout << "Path starts and is >>> " << path_start << "\n";
		//Shortest absolute form: "a", 1 char
		//Authority must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
		if (token.size() < 1 || path_start != string::npos || params_start != string::npos || frag_start != string::npos){
			cout << "\n\tRequest error: Invalid URI scheme (authority form)\n";
			return (true);
		}
	}

	//Origin Form doesn't have specific parsing since it's in included in Authority and Absolute forms
	if (invalid_values(token, rq_uri, path_start, params_start, frag_start, &form)){
			return (true);
	}
	return (false);
}


bool invalid_start_line(vector<string> const &line, URI &rq_uri)
{
	//Checking total tokens
	if (line.size() != 3){
		cout << "Request error: Invalid start-line tokens\n";
		return (true);
	}

	//Checking protocol token
	if (line[2].compare("HTTP/1.1") != 0){
		cout << "Request error: Invalid HTTP protocol version\n";
		return (true);
	}

	//Checking method token
	char	mth;
	line[0] == "GET" ? mth = 'g' :
		line[0] == "POST" ? mth = 'p' : 
		line[0] == "DELETE" ? mth = 'd' :
		mth = 'u';
	if (mth == 'u')
	{
		cout << "Request error: Invalid HTTP method\n";
		return (true);
	}

	//Checking URI tokens
	rq_uri.setMethod(mth);
	if (invalid_uri(line[1], rq_uri)){
		cout << "Request error: Invalid URI\n";
		return (true);
	}
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

bool invalid_header(vector<vector<string> > &tokens, URI &rq_uri)
{
	//rfc7230 says that no whitespace is accepted between field-name and :
	//Therefore, this is the syntax:			field-name: OWS field-content OWS
	//OWS = Optional WhiteSpace						/r/n
	size_t	pos;
	vector<vector<string> >::const_iterator	it;
	vector<string>::const_iterator	jt;
	for (it = tokens.begin() + 1; it != tokens.end(); ++it){
		for (jt = it->begin(); jt != it->end(); ++jt){
			pos = jt[0].find_first_of(':'); 	
			if (jt->size() == 1 && jt[0].size() >= 3 &&
					jt[0].back() != ':' && pos == jt[0].find_last_of(':')){
				rq_uri.getHeaders().insert(std::make_pair(jt[0].substr(0, pos-1), jt[0].substr(pos+1, jt[0].length())));
			}
			else if (jt->size() == 2 && jt[0].size() >= 2 &&
					jt[0].back() == ':' && pos == jt[0].find_last_of(':')){
				rq_uri.getHeaders().insert(std::make_pair(jt[0].substr(0, pos-1), jt[1]));
			}
			else {
				return (false);
			}
		}
	}
	return (false);
}

void	tokenizer(string &raw_request, vector<vector<string> >	&tokens)
{
	std::stringstream 	r_rq(raw_request);
	string							line;

	while (std::getline(r_rq, line))
	{
		vector<string>			words;
		string							word;
		std::stringstream		line_stream(line);
		cout << "Linee is [" << line << "] with length " << line.length() << "\n";

		while (line_stream >> word)
		{
			words.push_back(word);
		}
		tokens.push_back(words);
	}
}

bool	invalid_carriage_return(const char *rq){
	string	r;
	r += rq;

	size_t	end = r.find("\r\n\r\n");
	if (end == string::npos){
		return (true);
	}
	size_t	n = 0;
	while (n != string::npos && n < end){
		n = r.find("\n", n);
		//cout << "Enterrs\n n is: " << n << "\n";
		//cout << "end is: " << end << "\n";
		if ((n != string::npos && n >= 1 && r[n-1] != '\r') || n < 1){
			return (true);
		}
		++n;
	}
	return (false);
}

bool invalid_request(const char *request)
{
	if (request == nullptr){
		return (1);
	}

	URI	rq_uri;
	string	raw_request(request);
	//cout << "\nValidating following request of size [" << raw_request.size() << "]:\n[" << raw_request << "]\n\n";

	if (invalid_carriage_return(request)){
		cout << "Request error: Invalid carriage returns\n";
		return (true);
	}
	//Creating a container for lines and words
	vector<vector<string> >	tokens;
	tokenizer(raw_request, tokens);

	for (size_t i = 0; i < tokens.size(); ++i){
		cout << "Line [" << i << "]:\n";
		for (size_t j = 0; j < tokens[i].size(); ++j){
			cout << "\tWord [" << j << "]:" << tokens[i][j] << " with size " << tokens[i][j].size() << "\n";
		}
	}

	if (tokens.size() < REQUEST_MIN_LINES){
		return (1);
	}

	if (invalid_start_line(tokens[0], rq_uri)){
		cout << "Request error: Invalid start line\n";
		return (true);
	}

	if (invalid_header(tokens, rq_uri)){
		cout << "Request error: Invalid header fields\n";
		return (true);
	}

	return (false);
}



/*
int	main()
{
	if (invalid_request("GET http://localhost:80/index?key1=value1&key2=value2#section1  \r\nJuan:Paco\r\nPedro:  Pablo\r\n\r\nCoooonteent"))
	{
		return (1);
	}
	return (0);
}*/
