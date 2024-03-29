#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
#include <map>
#include <vector>
#include "../inc/Server.hpp"
#include "../inc/ServerException.hpp"

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
		string			request;
		string			body;
		bool				closeConnection;
		bool				headers_parsed;
		char				method;
		string			scheme;
		string			authority;
		string			host;
		size_t			port;
		string			path;
		string			query;
		string			fragment;
		size_t			headers_size;
		bool				isChunked;
		size_t			chunkSize;
		bool				expect_continue;
		mapStrStr		params;
		mapStrVect	headers;

////// Esto es de joselito
		std::string rawBody;
    std::string uri;
    std::string version;

		std::string	method2;
   	HttpStatusCode  statusCode;
		std::string contentType;
		std::string	boundary;

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

/////////////Esto es de Joselitoo
		std::string getRawBody() const;
    std::string getUri() const;
    std::string getVersion() const;

		std::string getMethod2() const;
		HttpStatusCode getStatusCode() const ;
		std::string getContentType() const;
		std::string getBoundary() const;

////////////////////////////////////////////////

		//Overloads
		URI	&operator=(const URI &rhs);

};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

//	../src/request_parser.cpp
bool	invalid_request(const char *request, URI &rq_uri);
bool	invalid_carriage_return(const char *rq, URI &rq_uri);
void	tokenizer(string &raw_request, vector<vector<string> >	&tokens);
bool	invalid_start_line(vector<string> const &line, URI &rq_uri);
bool	invalid_uri(const string &token, URI &rq_uri);
bool	invalid_header(vector<vector<string> > &tokens, URI &rq_uri);

//	../src/request_parser_utils.cpp
bool	invalid_chars(const string &path);
bool	invalid_values(const string &token, URI &rq_uri, size_t p_start, size_t pr_start, size_t f_start, char *form);
bool	invalid_authority(const string &auth, URI &rq_uri);
bool	invalid_query_syntax(const string &query, URI &rq_uri);
bool	invalid_port(const string &port, URI &rq_uri);
void	determine_uri_form(const string &uri, char *form);

//	../src/request_parser_tester.cpp
bool	request_testing();

//	../src/read_chunked.cpp
void	transfer_encoding(URI &rq_uri);
bool	read_chunked(int &client, URI &rq);

#endif

