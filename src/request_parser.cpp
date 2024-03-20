
#include	<vector>
#include	<string>
#include	<sstream>
#include	"../inc/request_parser.hpp"

#define	MAX_REQUEST_SIZE 
#define	REQUEST_MIN_LINES 3		//start-line, header, /r/n

using	std::cout;

bool invalid_request(const char *request, URI &rq_uri)
{
	if (request == nullptr){
		return (true);
	}

	string	raw_request(request);

	if (invalid_carriage_return(request, rq_uri)){
		cout << "Request error: Invalid carriage returns\n";
		return (true);
	}
	//Creating a container for lines and words
	vector<vector<string> >	tokens;
	tokenizer(raw_request, tokens);

	if (tokens.size() < REQUEST_MIN_LINES){
		return (true);
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

bool	invalid_carriage_return(const char *rq, URI &rq_uri){
	string	r;
	r += rq;

	size_t	end = r.find("\r\n\r\n");
	if (end == string::npos){
		return (true);
	}
	size_t	headers_size = 0, n = 0;
	while ((n = r.find("\n", n)) != string::npos && n < end){
		//cout << "Enterrs\n n is: " << n << "\n";
		//cout << "end is: " << end << "\n";
		++headers_size;
		if ((n >= 1 && r[n-1] != '\r') || n < 1){
			return (true);
		}
		++n;
	}
	rq_uri.setHeadersSize(headers_size);
	return (false);
}

void	tokenizer(string &raw_request, vector<vector<string> >	&tokens){
	std::stringstream 	r_rq(raw_request);
	string							line;

	while (std::getline(r_rq, line)){
		vector<string>			words;
		string							word;
		std::stringstream		line_stream(line);
		//cout << "Linee is [" << line << "] with length " << line.length() << "\n";
		while (line_stream >> word){
			words.push_back(word);
		}
		tokens.push_back(words);
	}
}

bool invalid_start_line(vector<string> const &line, URI &rq_uri){
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
	if (mth == 'u'){
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

bool	invalid_uri(const string &token, URI &rq_uri){
	if (invalid_chars(token)){
		cout << "\n\tRequest error: Invalid URI (not properly encoded)\n";
		return (true);
	}

	cout << "URI to check is: [" << token << "]\n";
	//We need to determine the URI form: origin (o), absolute (a), authority (y) or asterisk
	char	form = 'u'; //u = undefined
	determine_uri_form(token, &form);
	//cout << "The URI form is: [" << form << "]\n";
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

bool invalid_header(vector<vector<string> > &tokens, URI &rq_uri)
{
	//rfc7230 says that no whitespace is accepted between field-name and :
	//Therefore, this is the syntax:			field-name: OWS field-content OWS
	//OWS = Optional WhiteSpace						/r/n
	size_t	pos;
	size_t	index = 0;

	mapStrVect			hdrs;
	vector<string>	values, line;
	vector<vector<string> >::const_iterator	it;
	string	key;
	for (it = tokens.begin(); it != tokens.end() && index++ <= rq_uri.getHeadersSize(); ++it){
		if (it == tokens.begin() || it->size() < 1){
			continue ;
		}
		line = *it;

		if ((pos = line[0].find_first_of(':')) == string::npos){
			return (true);
		}
		switch (line.size()){
			case 1:
				if (line[0].size() < 3 || line[0].front() == ':' || line[0].back() == ':' || pos != line[0].find_last_of(':')){
					return (true);
				}
				values.push_back(line[0].substr(pos+1, line[0].length()));
				break ;

			case 2:
				if (line[0].size() < 2 || line[0].front() == ':' || line[0].back() != ':' || pos != line[0].find_last_of(':')){
					return (true);
				}
				values.push_back(line[1]);
				break ;

			default :
				if (line[0].size() < 2 || line[0].front() == ':' || line[0].back() != ':' || pos != line[0].find_last_of(':')){
					return (true);
				}
				for (size_t word = 1; word < line.size(); ++word){
					if (((word == line.size() -1) && (line[word].back() == ',')) ||
							((word < line.size() - 1) && (line[word].back() != ',' || line[word].size() <= 1))){
						return (true);
					}
					if (word < line.size() - 1){
						values.push_back(line[word].substr(0, line[word].size() - 1));
					}
					else{
						values.push_back(line[word]);
					}
				}
				break ;
			}
			key = line[0].substr(0, pos);
			std::transform(key.begin(), key.end(), key.begin(), ::toupper);
			hdrs[key] = values;
			values.clear();
			key.clear();
	}
	if (hdrs.size() == 0){
		return (true);
	}
	rq_uri.setHeaders(hdrs);
	transfer_encoding(rq_uri);
	return (false);
}


