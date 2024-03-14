#pragma once

#include "idk.hpp"
#include "request_parser.hpp"

class   Cgi : public URI
{
private:
    Route		cgiRoute;
	std::string filename;
	std::string responseBody;
	std::map<std::string, std::string> responseHeaders;
	std::map<std::string, std::string> env;
	char **envp;
    
public:
    Cgi();
	Cgi(Route const & cgiRoute, std::string const & filename, URI const & req);
	Cgi(Cgi const& other);
	~Cgi();
	Cgi& operator=(Cgi const& other);
	std::string getResponseBody();
	std::map<std::string, std::string> getResponseHeaders();
	std::map<std::string, std::string> getEnv(URI const & req, std::string const & filename);
	char	**mapToArray(std::map <std::string, std::string> const & map);
	std::string getQuery(std::string const & uri);
	std::string getFileExt(std::string const &path);
	void executeCgi(URI const & req);
};