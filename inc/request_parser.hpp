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
};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

#endif

