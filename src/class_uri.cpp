#include	"../inc/request_parser.hpp"

URI::URI() : request(""), body(""), statusCode(""),
	goingToResponse(false), closeConnection(false),
	method('u'), scheme(""), authority(""), host(""), port(80), path(""), query(""), fragment(""),
	headers_parsed(false), headers_size(0), isChunked(false), isMultipart(false), chunkSize(0), expect_continue(false){
}


URI::URI(URI const &copy) :
	request(copy.request), body(copy.body),
	goingToResponse(copy.goingToResponse), closeConnection(copy.closeConnection),
	method(copy.method), scheme(copy.scheme), authority(copy.authority), host(copy.host), port(copy.port), path(copy.path), query(copy.query), fragment(copy.fragment), params(copy.params),
	headers_parsed(copy.headers_parsed), headers_size(copy.headers_size), isChunked(copy.isChunked), isMultipart(copy.isMultipart), chunkSize(copy.chunkSize), expect_continue(copy.expect_continue){

	for (mapStrVect::const_iterator it = copy.headers.begin(); it != copy.headers.end(); ++it) {
		vector<string> vecCopy(it->second.begin(), it->second.end());
		headers[it->first] = vecCopy;
	}
}

URI::~URI(){
}


//Getters & Setters
string	URI::getRequest()const{
	return (request);
}

string	URI::getBody()const{
	return (body);
}

bool	URI::getGoingToResponse()const{
	return (goingToResponse);
}

bool	URI::getCloseConnection()const{
	return (closeConnection);
}

bool	URI::getHeadersParsed()const{
	return (headers_parsed);
}

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

bool	URI::getIsChunked()const{
	return (isChunked);
}

bool	URI::getIsMultipart()const{
	return (isMultipart);
}

size_t	URI::getChunkSize()const{
	return (chunkSize);
}

bool	URI::getExpectContinue()const{
	return (expect_continue);
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

string	URI::getStatusCode() const{
    return (this->statusCode);
}

void	URI::setRequest(string &rq){
	this->request = rq;
}

void	URI::setBody(string &bd){
	this->body = bd;
}

void	URI::setGoingToResponse(bool go){
	goingToResponse = go;	
}

void	URI::setCloseConnection(bool close){
	this->closeConnection = close;
}

void	URI::setHeadersParsed(bool parsed){
	this->headers_parsed = parsed;
}

void	URI::setMethod(char mth){
	this->method = mth;
}

void	URI::setScheme(std::string sche){
	this->scheme = sche;
}

void	URI::setAuthority(std::string auth){
	this->authority = auth;
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
	this->query = qry;
}

void	URI::setIsChunked(bool chunked){
	this->isChunked = chunked;
}

void	URI::setIsMultipart(bool multi){
	this->isMultipart = multi;
}

void	URI::setChunkSize(size_t size){
	this->chunkSize = size;
}

void	URI::setExpectContinue(bool expect){
	this->expect_continue = expect;
}

void	URI::setParams(mapStrStr prms){
	this->params = prms;
}

void	URI::setFragment(std::string fragm){
	this->fragment = fragm;
}

void	URI::setHeadersSize(size_t size){
	this->headers_size = size;
}

void	URI::setHeaders(mapStrVect hdrs){
	this->headers = hdrs;
}

void	URI::setStatusCode(string status){
	this->statusCode = status;
}

void		URI::setBoundary(string bdr){
	this->boundary = bdr;
}
//Overloads
URI	&	URI::operator=(const URI &rhs){
	this->request = rhs.getRequest();
	this->method = rhs.getMethod();
	this->goingToResponse = rhs.getGoingToResponse();
	this->closeConnection = rhs.getCloseConnection();
	this->headers_parsed = rhs.getHeadersParsed();
	this->scheme = rhs.getScheme();
	this->authority = rhs.getAuthority();
	this->host = rhs.getHost();
	this->port = rhs.getPort();
	this->path = rhs.getPath();
	this->query = rhs.getQuery();
	this->headers_size = rhs.getHeadersSize();
	this->isChunked = rhs.getIsChunked();
	this->isMultipart = rhs.getIsMultipart();
	this->chunkSize = rhs.getChunkSize();
	this->expect_continue = rhs.getExpectContinue();
	this->params = rhs.getParams();
	this->fragment = rhs.getFragment();
	this->headers = rhs.getHeaders();
	return (*this);
}

std::ostream & 	operator<<(std::ostream & o, const URI &uri){
	o << "URI class: \n\tmethod:\t" << uri.getMethod() << "\n\tscheme:\t" << uri.getScheme() << "\n\tauthority:\t" << uri.getAuthority() << "\n\tpath:\t";
	o << uri.getPath() << "\n\tquery:\t" << uri.getQuery() << "\n\tfragment:\t"<< uri.getFragment() << "\n\tisChunked:\t"<< uri.getIsChunked() << "\n\n";


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
