
#include	<vector>
#include	<string>
#include	<sstream>
#include	"../inc/request_parser.hpp"


#define	MAX_REQUEST_SIZE 
#define	REQUEST_MIN_LINES 3							//start-line, header, /r/n
#define	CRLFx2						"\r\n\r\n"

using	std::cout;

bool	invalid_request(URI &rq, Server &server){
	cout << RED << ";;;;;;;;;;;;;;;;;;; PARSING HEADERS!!! ;;;;;;;;; " << EOC << std::endl;
	if (rq.getRequest().size() < 1){
		cout << RED << "Server Error: Couldn't read the request" << EOC << std::endl;
		rq.setStatusCode(STATUS_500);
		return (true);
	}

	if (invalid_carriage_return(rq)){
		cout << RED << "Request error: Invalid carriage returns" << EOC << std::endl;
		rq.setStatusCode(STATUS_400);
		return (true);
	}
	//Creating a container for lines and words
	vector<vector<string> >	tokens;
	tokenizer(rq.getRequest(), tokens);

	if (tokens.size() < REQUEST_MIN_LINES){
		cout << RED << "Request error: Not enough request lines" << EOC << std::endl;
		rq.setStatusCode(STATUS_400);
		return (true);
	}
	if (invalid_start_line(tokens[0], rq, server)){
		return (true);
	}
	if (invalid_header(tokens, rq, server)){
		cout << RED << "Request error: Invalid header fields" << EOC << std::endl;
		return (true);
	}

	rq.setHeadersParsed(true);
	return (false);
}

bool	invalid_carriage_return(URI &rq){
	string	r = rq.getRequest();

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
	rq.setHeadersSize(headers_size);
	return (false);
}

void	tokenizer(string raw_request, vector<vector<string> >	&tokens){
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


///////////////////////////Esto es de Joselitoo

std::string URI::getUri() const {
    return uri;
}

std::string URI::getVersion() const {
    return version;
}

std::string URI::getRawBody() const {
    return rawBody;
}


std::string URI::getContentType() const
{
    return contentType;
}

std::string URI::getBoundary() const
{
    return boundary;
}
////////////////////////////////////////////////////
