#include	"../inc/request_parser.hpp"

URI::URI() :
	scheme(nullptr), authority(nullptr), path(nullptr), params(nullptr), fragment(nullptr){
}

URI::URI(std::string sche, std::string auth, std::string path, std::string prms, std::string fragm) :
	scheme(sche), authority(auth), path(path), params(prms), fragment(fragm){
}

URI::URI(URI const &copy) :
	scheme(copy.scheme), authority(copy.authority), path(copy.path), params(copy.params), fragment(copy.fragment){
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

std::string	URI::getPath()const{
	return (path);
}

std::string	URI::getParams()const{
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

void	URI::setPath(std::string path){
	this->path = path;
}

void	URI::setParams(std::string prms){
	params = prms;
}

void	URI::setFragment(std::string fragm){
	fragment = fragm;
}

//Overloads
URI	&	URI::operator=(const URI &rhs){
	this->scheme = rhs.getScheme();
	this->authority = rhs.getAuthority();
	this->path = rhs.getPath();
	this->params = rhs.getParams();
	this->fragment = rhs.getFragment();
	return (*this);
}

std::ostream & 	operator<<(std::ostream & o, const URI &uri){
	o << "URI class: \n\tscheme:\t" << uri.getScheme() << "\n\tauthority:\t" << uri.getAuthority() << "\n\tpath:\t";
	o << uri.getPath() << "\n\tparams:\t" << uri.getParams() << "\n\tfragment:\t"<< uri.getFragment() << "\n\n";
	return o;
}
