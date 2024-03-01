#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
#include <map>

using	std::string;


bool invalid_request(const char *request);

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

		//Overloads
		URI	&operator=(const URI &rhs);
};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

#endif

