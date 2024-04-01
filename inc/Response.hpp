#pragma once

#include "ServerException.hpp"
#include "request_parser.hpp"
#include "Mime.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "Config.hpp"
#include "Cgi.hpp"
#include "Utils.hpp"
#include <sys/stat.h>

using	std::string;

class   URI;

class Response
{
    private:
        URI 		*request;
        string	code;
        Mime 		mime;
        std::map<string, string> headers;
        string	body;
        bool		isListing;

    public:
        //Response(string const &buffer);
        Response();
        ~Response();

				//Getters & Setter
        Server 	getServer();
        Route  	getRoute(Server &);
        string	getRequestedResource(string const & uri);
        Route		findBestMatchInRoute(Route & route, string const & resource);
        Route		findBestMatchInServer(Server & server, string const & resource);
        string	getFilePath(Server const & server, Route const & route);
        string	tryFiles(Server const & server, Route const & route, string & root);

        void		readContent(string const &filePath, string code);
        void		checkMethods(Route const & route);
        void 		checkRedirection(Route const & route);

        void 		handleGet(Server const &server, Route const &route);
        void 		handlePost(Server const &server, Route const &route);
        void 		handleDelete(Server const &server, Route const &route);

        void 		handleResponse(void);

        void 		readBody(Route const & route);
        void 		processUrlEncodedBody(const string& body);
        void 		processMultipartFormDataBody(const string& body, Route const & route);
        void 		processFileUpload(std::istringstream& ss, const string& line, Route const & route);
        void 		processFormField(std::istringstream& ss, const string& line, std::map<string, string>& queryStrings);
        Route 	deepSearch(Server & server, string const & resource);
};

void 											removeConsecutiveChars(string & s, char c);
std::pair<string, bool>   getMatchedPath(string serverRootPath, string path);
bool 											CheckIfInDirectory(string path);
bool 											CheckIfInFile(string path);
