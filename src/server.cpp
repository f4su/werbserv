#include "../inc/Server.hpp"
#include "../inc/ServerException.hpp"
#include "../inc/Utils.hpp"

#define DEFAULT_ROOT	"/var/www/html"
#define MAG						"\033[1;35m"
#define EOC						"\033[0m"
#define CYA						"\033[36m"

int Server::max = 0;

Server::Server() : socket(-1), clients(), port(-1), host(0), serverNames(), clientMaxBodySize(0),
	root(DEFAULT_ROOT), index(), errorPages(), allowListing(false), routes(){
};

Server::~Server(){
};


//----------------
//		Getters
//----------------

int	Server::getMax(){
	return max;
}

int Server::getSocket() const{
    return socket;
}

std::vector<int>	Server::getClients() const{
	return clients;
}

int Server::getPort() const{
    return port;
}

uint32_t	Server::getHost() const{
    return host;
}

std::vector<std::string>    Server::getServerNames() const{
    return serverNames;
}

size_t  Server::getClientMaxBodySize() const{
    return clientMaxBodySize;
}

std::string Server::getRoot() const{
    return root;
}

std::vector<std::string>    Server::getIndex() const{
    return index;
}

bool    Server::getAllowListing() const{
    return allowListing;
}

std::map<int, std::string>  Server::getErrorPages() const{
    return errorPages;
}

std::vector<Route>  Server::getRoutes() const{
    return (this->routes);
}


//----------------
//		Setters
//----------------

void	Server::setMax(int const &m){
	max = m;
}

void    Server::setSocket(const int & socket){
    this->socket = socket;
}

bool	Server::setClients(const int& clnt){
	if (clnt < 0){
		return (false);
	}
	clients.push_back(clnt);
	return (true);
}

void	Server::setPort(const int & port){
    this->port = port;
}

bool	Server::setHost(const std::string & host){
	if (host.empty()){
		return (true);
	}

	uint32_t ip_addr = 0;
	int octet = 0;
	int octet_index = 0;
	const char *ip_str = host.c_str();

	while (*ip_str != '\0') {
			if (*ip_str >= '0' && *ip_str <= '9') {
					octet = octet * 10 + (*ip_str - '0');
			}
			else if (*ip_str == '.') {
					if (octet < 0 || octet > 255) {
						return false;
					}
					// Shift the octet to its appropriate position
					ip_addr |= (uint32_t)octet << (24 - octet_index * 8);
					octet = 0;
					octet_index++;
			}
			else {
				return false;
			}
			ip_str++;
	}

	if (octet < 0 || octet > 255) {
			return false;
	}
	ip_addr |= (uint32_t)octet << (24 - octet_index * 8);

	std::cout << "Address isssssssssssss -> " << ip_addr << std::endl;
  this->host = ip_addr; 
	return (false);
}

void	Server::setServerNames(const std::vector<std::string>& serverNames){
    this->serverNames = serverNames;
}

void	Server::setClientMaxBodySize(const size_t & clientMaxBodySize){
    this->clientMaxBodySize = clientMaxBodySize;
}

void	Server::setRoot(const std::string & root){
    this->root = root;
}

void    Server::setAllowListing(const bool & allowListing){
    this->allowListing = allowListing;
}

void    Server::setErrorPages(const std::map<int, std::string>& errorPages){
    this->errorPages = errorPages;
}

void    Server::setIndex(const std::vector<std::string>& index){
	this->index = index;
}

void    Server::addRoute(Route route){
    this->routes.push_back(route);
}


//----------------
//		Functions
//----------------

void	Server::print() const{
		static size_t	nb = 0;
    std::cout << MAG << "\nv------" << " Server " << ++nb << " --------v\n" << EOC << std::endl;
    if (port > 0)
        std::cout << CYA << "port: " << EOC << port << std::endl;
    if (host != 0)
        std::cout << CYA << "host: " << EOC << getHostStr() << std::endl;
    if (!serverNames.empty()) {
        std::cout << CYA << "server_names: " << EOC;
        printContainer(serverNames);
    }
    if (clientMaxBodySize > 0)
        std::cout << CYA << "client_max_body_size: " << EOC << clientMaxBodySize << std::endl;
    if (!root.empty())
        std::cout << CYA << "root: " << EOC << root << std::endl;
    if (!index.empty()) {
        std::cout << CYA << "index: " << EOC;
        printContainer(index);
    }
    if (!errorPages.empty()) {
        std::cout << CYA << "error_pages: " << EOC;
        printMap(errorPages);
    }
    std::cout << CYA << "allow_listing: " << EOC << allowListing << std::endl;
    if (!routes.empty()) {
        std::cout << CYA << "routes: " << EOC << std::endl;
        for (std::vector<Route>::const_iterator it = routes.begin(); it != routes.end(); ++it)
            (*it).print();
    }
    std::cout << MAG << "\n^--------------------^\n" << EOC << std::endl;
}

void Server::fill(std::string const &line, int &lineNb){
    std::vector<std::string>    split;
    std::string                 option, value;

    split = ft_split(line, "=");

    if (split.size() != 2)
        throw ServerException("Invalid server line", lineNb);
    option = split[0];
    value = split[1];

    if (value.empty() || option.empty())
        throw ServerException("Invalid server line", lineNb);
    if (option == "port" && port == -1) {
        char *end;
        size_t port = std::strtod(value.c_str(), &end);
        if (*end != '\0')
            throw ServerException("Invalid server line", lineNb); 
        setPort(port);
    }
    else if (option == "host" && host == 0) {
        if (setHost(value)){
        	throw ServerException("Invalid host address");   
				}
    }
    else if (option == "server_names" && serverNames.empty()) {
        std::vector<std::string>    names = ft_split(value, ", ");
        if (names.empty())
            throw ServerException("Invalid server line", lineNb);
        setServerNames(names);
    }
    else if (option == "client_max_body_size" && clientMaxBodySize == 0) {
        char *end;
        double size = std::strtod(value.c_str(), &end);
        if (*end != '\0')
            throw ServerException("Invalid body size", lineNb);   
        setClientMaxBodySize(size);
    }
    else if (option == "root" && root == DEFAULT_ROOT) {
        setRoot(value);
    }
    else if (option == "index" && index.empty()) {
        std::vector<std::string>    index = ft_split(value, ", ");
        if (index.empty())
            throw ServerException("Invalid server line", lineNb);
        setIndex(index);
    }
    else if (option == "error_pages" && errorPages.empty()) {
        if (!mapErrorPages(errorPages, value))
            throw ServerException("Invalid server line", lineNb);
    }
    else if (option == "allow_listing" && allowListing == false) {
        if (value == "on" || value == "1" || value == "true")
            setAllowListing(true);
        else if (value == "off" || value == "0" || value == "false")
            setAllowListing(false);
        else
            throw ServerException("Invalid server line", lineNb);
    }
    else   
        throw ServerException("Invalid server option", lineNb);
}


Server::iterator Server::begin() {
	return routes.begin();
}

Server::iterator Server::end(){
	return routes.end();
}


void	Server::remove_client(const int & client){
	std::vector<int>::iterator position;

	position = std::find(clients.begin(), clients.end(), client);
	if (position != clients.end()){
		clients.erase(position);
	}
}


std::string Server::getHostStr()const{
    std::string ip_str;

		if (host == 0){
			ip_str += "Not set";
			return (ip_str);
		}

    // Extract each octet from the IP address
    uint8_t octet1 = (host >> 24) & 0xFF;
    uint8_t octet2 = (host >> 16) & 0xFF;
    uint8_t octet3 = (host >> 8) & 0xFF;
    uint8_t octet4 = host & 0xFF;

    // Convert octets to string and concatenate them with dots
    ip_str += std::to_string(octet1) + ".";
    ip_str += std::to_string(octet2) + ".";
    ip_str += std::to_string(octet3) + ".";
    ip_str += std::to_string(octet4);

    return ip_str;
}
