#ifndef	REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP	

#include <iostream>
using	std::string;

class	URI{
	private:
		string	scheme;
		string	authority;
		string	path;
		string	params;
		string	fragment;

	public:

		//Constructors & Destructors
		URI();
		URI(string sche, string auth, string path, string prms, string fragm);
		URI(URI const &copy);
		~URI();

		//Getters & Setters
		string	getScheme()const;
		string	getAuthority()const;
		string	getPath()const;
		string	getParams()const;
		string	getFragment()const;
		void	setScheme(string sche);
		void	setAuthority(string auth);
		void	setPath(string path);
		void	setParams(string prms);
		void	setFragment(string fragm);

		//Overloads
		URI	&operator=(const URI &rhs);
};

//Overloads
std::ostream & 	operator<<(std::ostream &o, const URI &uri);

#endif

