#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
#include <map>

using	std::string;
using	std::vector;

class	URI{
	private:
		char			method;
		string		scheme;
		string		authority;
		string		host;
		size_t		port;
		string		path;
		string		query;
		string		fragment;
		std::map<string, string>	params;
		std::map<string, string>	headers;
		std::string rawBody;
    	std::string uri;
    	std::string version;
		std::string body;

		////////////////////////////////////////
		std::string	method2;
		///////////////////////////////////////////

	public:

		//Constructors & Destructors
		URI();
		URI(string sche, string auth, string host, size_t prt, string pth, string qry, std::map<string, string> prms, string fragm, std::map<string, string> hdrs);
		URI(URI const &copy);
		~URI();

		//Getters & Setters
		char			getMethod()const;
		string		getScheme()const;
		string		getHost()const;
		size_t		getPort()const;
		string		getAuthority()const;
		string		getPath()const;
		string		getQuery()const;
		string		getFragment()const;
		std::map<string, string>	getParams()const;
		std::map<string, string>	getHeaders()const;
		void	setMethod(char mth);
		void	setScheme(string sche);
		void	setAuthority(string auth);
		void	setHost(string host);
		void	setPort(size_t port);
		void	setPath(string path);
		void	setQuery(string qry);
		void	setFragment(string fragm);
		void	setParams(std::map<string, string> prms);
		void	setHeaders(std::map<string, string> hdrs);
		std::string getRawBody() const;
    	std::string getUri() const;
    	std::string getVersion() const;

		//Overloads
		URI	&operator=(const URI &rhs);

		///////////////////////////////////////////////
		std::string getMethod2() const;
		////////////////////////////////////////////////
};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

//	../src/request_parser.cpp
bool	invalid_request(const char *request, URI &rq_uri);
bool	invalid_carriage_return(const char *rq);
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

#endif

