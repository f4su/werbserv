#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
#include <map>
#include <vector>

#define RED "\033[1;31m"
#define CYA	"\033[36m"
#define MAG	"\033[1;35m"
#define EOC	"\033[0m"

using	std::string;
using	std::vector;
using	std::map;

typedef map<string, vector<string> >	mapStrVect;
typedef map<string, string>						mapStrStr;

class	URI{
	private:
		char				method;
		string			scheme;
		string			authority;
		string			host;
		size_t			port;
		string			path;
		string			query;
		string			fragment;
		size_t			headers_size;
		mapStrStr		params;
		mapStrVect	headers;

	public:

		//Constructors & Destructors
		URI();
		URI(string sche, string auth, string host, size_t prt, string pth, string qry, map<string, string> prms, string fragm, size_t headers_size, map<string, vector<string> > hdrs);
		URI(URI const &copy);
		~URI();

		//Getters & Setters
		char				getMethod()const;
		string			getScheme()const;
		string			getHost()const;
		size_t			getPort()const;
		string			getAuthority()const;
		string			getPath()const;
		string			getQuery()const;
		string			getFragment()const;
		size_t			getHeadersSize()const;
		mapStrStr		getParams()const;
		mapStrVect	getHeaders()const;
		void				setMethod(char mth);
		void				setScheme(string sche);
		void				setAuthority(string auth);
		void				setHost(string host);
		void				setPort(size_t port);
		void				setPath(string path);
		void				setQuery(string qry);
		void				setFragment(string fragm);
		void				setHeadersSize(size_t size);
		void				setParams(map<string, string> prms);
		void				setHeaders(map<string, vector<string> > hdrs);

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

#endif

