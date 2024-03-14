#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
#include <map>

using	std::string;

class	URI{
	private:
		string		scheme;
		string		authority;
		string		host;
		string		port;
		string		path;
		string		query;
		std::map<string, string>	params;
		string		fragment;

		////////////////////////////////////////
		std::string	method;
    	std::string uri;
    	std::string version;
    	std::string host;
    	int port2;
    	std::map<std::string, std::string> headers;
    	std::string body;
    	std::string rawBody;
		///////////////////////////////////////////

	public:

		//Constructors & Destructors
		URI();
		URI(string sche, string auth, string path, string query, string fragm);
		URI(URI const &copy);
		~URI();

		//Getters & Setters
		string		getScheme()const;
		string		getAuthority()const;
		string		getPath()const;
		string		getQuery()const;
		std::map<string, string>	getParams()const;
		string		getFragment()const;
		void	setScheme(string sche);
		void	setAuthority(string auth);
		void	setHost(string host);
		void	setPort(string port);
		void	setPath(string path);
		void	setQuery(string query);
		void	setParams(std::map<string, string> prms);
		void	setFragment(string fragm);

		//Overloads
		URI	&operator=(const URI &rhs);

		///////////////////////////////////////////////
		std::string getMethod() const;
    	std::string getUri() const;
    	std::string getVersion() const;
    	std::map<std::string, std::string> getHeaders() const;
    	std::string getRawBody() const;
		std::string getHost() const;
    	int getPort2() const;
		////////////////////////////////////////////////
};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

#endif

