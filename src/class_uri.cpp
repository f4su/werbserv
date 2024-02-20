#include	"../inc/request_parser.hpp"

URI::URI() :
	scheme(nullptr), authority(nullptr), host(nullptr), port(nullptr), path(nullptr), query(nullptr), params(nullptr), fragment(nullptr){
}

URI::URI(std::string sche, std::string auth, std::string host, std::string port, std::string path, std::string query, std::map<string, string> params, std::string fragm) :
	scheme(sche), authority(auth), host(host), port(port), path(path), query(query), params(params), fragment(fragm){
}

URI::URI(URI const &copy) :
	scheme(copy.scheme), authority(copy.authority), host(copy.host), port(copy.port), path(copy.path), query(copy.query), params(copy.params), fragment(copy.fragment){
}

URI::~URI(){
}


//Getters & Setters
std::string	URI::getScheme()const{
	return (scheme);
}

std::string	URI::getAuthority()const{
	return (authority);
}

std::string	URI::getHost()const{
	return (host);
}

std::string	URI::getPort()const{
	return (port);
}

std::string	URI::getPath()const{
	return (path);
}

std::string	URI::getQuery()const{
	return (query);
}

std::map<string, string>	URI::getParams()const{
	return (params);
}

std::string	URI::getFragment()const{
	return (fragment);
}

void	URI::setScheme(std::string sche){
	scheme = sche;
}

void	URI::setAuthority(std::string auth){
	authority = auth;
}

void	URI::setHost(std::string host){
	this->host = host;
}

void	URI::setPort(std::string port){
	this->port = port;
}

void	URI::setPath(std::string path){
	this->path = path;
}

void	URI::setQuery(std::string query){
	query = query;
}

void	URI::setParams(std::map<string, string> prms){
	params = prms;
}

void	URI::setFragment(std::string fragm){
	fragment = fragm;
}

//Overloads
URI	&	URI::operator=(const URI &rhs){
	this->scheme = rhs.getScheme();
	this->authority = rhs.getAuthority();
	this->host = rhs.getHost();
	this->port = rhs.getPort();
	this->path = rhs.getPath();
	this->query = rhs.getQuery();
	this->params = rhs.getParams();
	this->fragment = rhs.getFragment();
	return (*this);
}

std::ostream & 	operator<<(std::ostream & o, const URI &uri){
	o << "URI class: \n\tscheme:\t" << uri.getScheme() << "\n\tauthority:\t" << uri.getAuthority() << "\n\tpath:\t";
	o << uri.getPath() << "\n\tquery:\t" << uri.getQuery() << "\n\tfragment:\t"<< uri.getFragment() << "\n\n";
	return o;
}
