#include	"../inc/request_parser.hpp"

URI::URI() :
	scheme(""), authority(""), host(""), port(80), path(""), query(""), fragment(""){
	params[""] = "";
	headers[""] = "";
}

URI::URI(std::string sche, std::string auth, std::string host, size_t prt, std::string pth, std::string qry,
		std::map<string, string> prms, std::string fragm, std::map<string, string> hdrs) :
	scheme(sche), authority(auth), host(host), port(prt), path(pth), query(qry), fragment(fragm), params(prms), headers(hdrs){
}

URI::URI(URI const &copy) :
	scheme(copy.scheme), authority(copy.authority), host(copy.host), port(copy.port), path(copy.path),
	query(copy.query), fragment(copy.fragment), params(copy.params), headers(copy.headers){
}

URI::~URI(){
}


//Getters & Setters
char	URI::getMethod()const{
	return (method);
}

std::string	URI::getScheme()const{
	return (scheme);
}

std::string	URI::getAuthority()const{
	return (authority);
}

std::string	URI::getHost()const{
	return (host);
}

size_t URI::getPort()const{
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

std::map<string, string>	URI::getHeaders()const{
	return (headers);
}

void	URI::setMethod(char mth){
	method = mth;
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

void	URI::setPort(size_t port){
	this->port = port;
}

void	URI::setPath(std::string path){
	this->path = path;
}

void	URI::setQuery(std::string qry){
	query = qry;
}

void	URI::setParams(std::map<string, string> prms){
	params = prms;
}

void	URI::setFragment(std::string fragm){
	fragment = fragm;
}

void	URI::setHeaders(std::map<string, string> hdrs){
	headers = hdrs;
}

//Overloads
URI	&	URI::operator=(const URI &rhs){
	this->method = rhs.getMethod();
	this->scheme = rhs.getScheme();
	this->authority = rhs.getAuthority();
	this->host = rhs.getHost();
	this->port = rhs.getPort();
	this->path = rhs.getPath();
	this->query = rhs.getQuery();
	setParams(rhs.getParams());
	this->fragment = rhs.getFragment();
	this->headers = rhs.getHeaders();
	return (*this);
}

std::ostream & 	operator<<(std::ostream & o, const URI &uri){
	o << "URI class: \n\tmethod:\t" << uri.getMethod() << "\n\tscheme:\t" << uri.getScheme() << "\n\tauthority:\t" << uri.getAuthority() << "\n\tpath:\t";
	o << uri.getPath() << "\n\tquery:\t" << uri.getQuery() << "\n\tfragment:\t"<< uri.getFragment() << "\n\n";


	std::map<string, string>	const hdrs = uri.getHeaders();
	std::map<string, string>::const_iterator	it;
	o << "Headers:\n";
	if (hdrs.size() > 0){
		for (it = hdrs.cbegin(); it != hdrs.cend(); ++it){
			o << "[" << it->first << "] = [" << it->second << "]\n";
		}
	}
	o << "\n";

	std::map<string, string>	const prms = uri.getParams();
	std::map<string, string>::const_iterator	jt;
	o << "Params:\n";
	if (prms.size() > 0){
		for (jt = prms.cbegin(); jt != prms.cend(); ++jt){
			o << "[" << jt->first << "] = [" << jt->second << "]\n";
		}
	}
	o << "\n";
	return o;
}
