#include	"../inc/request_parser.hpp"

URI::URI() : scheme(""), authority(""), host(""), port(80), path(""), query(""), fragment(""), headers_size(0){
}

URI::URI(std::string sche, std::string auth, std::string host, size_t prt,
		std::string pth, std::string qry, mapStrStr prms,
		std::string fragm, size_t headers_size, mapStrVect hdrs) :
	scheme(sche), authority(auth), host(host), port(prt), path(pth), query(qry), fragment(fragm),
	headers_size(headers_size), params(prms){

	for (mapStrVect::const_iterator it = hdrs.begin(); it != hdrs.end(); ++it) {
		vector<string> vecCopy(it->second.begin(), it->second.end());
		headers[it->first] = vecCopy;
	}
}

URI::URI(URI const &copy) :
	scheme(copy.scheme), authority(copy.authority), host(copy.host), port(copy.port), path(copy.path),
	query(copy.query), fragment(copy.fragment), headers_size(copy.headers_size), params(copy.params){

	for (mapStrVect::const_iterator it = copy.headers.begin(); it != copy.headers.end(); ++it) {
		vector<string> vecCopy(it->second.begin(), it->second.end());
		headers[it->first] = vecCopy;
	}
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

mapStrStr	URI::getParams()const{
	return (params);
}

std::string	URI::getFragment()const{
	return (fragment);
}


size_t		URI::getHeadersSize()const{
	return (headers_size);
}

mapStrVect	URI::getHeaders()const{
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

void	URI::setParams(mapStrStr prms){
	params = prms;
}

void	URI::setFragment(std::string fragm){
	fragment = fragm;
}

void	URI::setHeadersSize(size_t size){
	headers_size = size;
}

void	URI::setHeaders(mapStrVect hdrs){
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
	this->headers_size = rhs.getHeadersSize();
	this->params = rhs.getParams();
	this->fragment = rhs.getFragment();
	this->headers = rhs.getHeaders();
	return (*this);
}

std::ostream & 	operator<<(std::ostream & o, const URI &uri){
	o << "URI class: \n\tmethod:\t" << uri.getMethod() << "\n\tscheme:\t" << uri.getScheme() << "\n\tauthority:\t" << uri.getAuthority() << "\n\tpath:\t";
	o << uri.getPath() << "\n\tquery:\t" << uri.getQuery() << "\n\tfragment:\t"<< uri.getFragment() << "\n\n";


	mapStrVect	hdrs = uri.getHeaders();
	mapStrVect::const_iterator	it;
	o << CYA << "Headers (" << hdrs.size() << "):" << EOC << std::endl;
	if (hdrs.size() > 0){
		size_t	idx = 1;
		for (it = hdrs.begin(); it != hdrs.end(); ++it){
			o << CYA << "\t" << idx++ << ". " << EOC << "[" << it->first << "] = ";

			for (vector<string>::const_iterator i = it->second.begin(); i != it->second.end(); ++i){
				o << "[" << *i << "]  ";
			}
			o << std::endl;
		}

	}
	o << std::endl;

	mapStrStr	const prms = uri.getParams();
	mapStrStr::const_iterator	jt;
	o << CYA << "Params (" << prms.size() << "):" << EOC << std::endl;
	if (prms.size() > 0){
		size_t	idx = 1;
		for (jt = prms.cbegin(); jt != prms.cend(); ++jt){
			o << "\t" << CYA << idx++ << ". " << EOC << "[" << jt->first << "] = [" << jt->second << "]" << std::endl;
		}
	}
	o << "\n";
	return o;
}
