#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
#include <map>
#include <vector>
#include "Server.hpp"
#include "ServerException.hpp"

#include "status_codes.hpp"

#define RED "\033[1;31m"
#define CYA	"\033[36m"
#define MAG	"\033[1;35m"
#define EOC	"\033[0m"

using	std::string;
using	std::vector;
using	std::map;

typedef map<string, vector<string> >	mapStrVect;
typedef map<string, string>						mapStrStr;

class	Server;

class	URI
{
	private:
		//	Request - Response
		string			request;
		string			body;
   	string			statusCode;

		//	Utils
		bool				closeConnection;

		//	URI
		char				method;
		string			scheme;
		string			authority;
		string			host;
		size_t			port;
		string			path;
		string			query;
		string			fragment;
		mapStrStr		params;

		//	HEADERS
		bool				headers_parsed;
		size_t			headers_size;
		bool				isChunked;
		size_t			chunkSize;
		bool				expect_continue;
		mapStrVect	headers;
		

////// Esto es de joselito
		string rawBody;
    string uri;
    string version;

		string	method2;
		string	contentType;
		string	boundary;

///////////////////////////////////////////

	public:

		//Constructors & Destructors
		URI();
		URI(URI const &copy);
		~URI();

		//Getters & Setters
		string			getRequest()const;
		string			getBody()const;
		bool				getCloseConnection()const;
		bool				getHeadersParsed()const;
		char				getMethod()const;
		string			getScheme()const;
		string			getHost()const;
		size_t			getPort()const;
		string			getAuthority()const;
		string			getPath()const;
		string			getQuery()const;
		string			getFragment()const;
		size_t			getHeadersSize()const;
		bool				getIsChunked()const;
		size_t			getChunkSize()const;
		bool				getExpectContinue()const;
		mapStrStr		getParams()const;
		mapStrVect	getHeaders()const;
		string			getStatusCode() const;
		void				setRequest(string &rq);
		void				setBody(string &bd);
		void				setCloseConnection(bool close);
		void				setHeadersParsed(bool parsed);
		void				setMethod(char mth);
		void				setScheme(string sche);
		void				setAuthority(string auth);
		void				setHost(string host);
		void				setPort(size_t port);
		void				setPath(string path);
		void				setQuery(string qry);
		void				setFragment(string fragm);
		void				setHeadersSize(size_t size);
		void				setIsChunked(bool chunked);
		void				setChunkSize(size_t size);
		void				setExpectContinue(bool expect);
		void				setParams(map<string, string> prms);
		void				setHeaders(map<string, vector<string> > hdrs);
		void				setStatusCode(string status);

/////////////Esto es de Joselitoo
		string	getRawBody() const;
    string	getUri() const;
    string	getVersion() const;

		string	getMethod2() const;
		string	getContentType() const;
		string	getBoundary() const;

////////////////////////////////////////////////

		//Overloads
		URI	&operator=(const URI &rhs);

};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

//	../src/request_parser.cpp
bool	invalid_request(URI &rq_uri);
bool	invalid_carriage_return(URI &rq_uri);
void	tokenizer(string raw_request, vector<vector<string> >	&tokens);

//	../src/request_parser_utils.cpp
bool	invalid_values(const string &token, URI &rq_uri, size_t p_start, size_t pr_start, size_t f_start, char *form);
bool	invalid_authority(const string &auth, URI &rq_uri);
bool	invalid_query_syntax(const string &query, URI &rq_uri);
bool	invalid_port(const string &port, URI &rq_uri);

//	../src/request_parser_start_line.cpp
bool	invalid_start_line(vector<string> const &line, URI &rq_uri);
bool	invalid_uri(const string &token, URI &rq_uri);
bool	invalid_chars(const string &path);
void	determine_uri_form(const string &uri, char *form);

//	../src/request_parser_headers.cpp
bool	invalid_header(vector<vector<string> > &tokens, URI &rq_uri);

//	../src/request_parser_tester.cpp
bool	request_testing();

//	../src/read_chunked.cpp
void	transfer_encoding(URI &rq_uri);
bool	read_chunked(int &client, URI &rq);

#endif

