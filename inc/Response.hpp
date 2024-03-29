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

class   URI;

class Response
{
    private:
        URI *request;
        HttpStatusCode code;
        Mime mime;
        std::map<std::string, std::string> headers;
        std::string body;
        bool isListing;

    public:
        //Response(std::string const &buffer);
        Response();
        ~Response();
        void handleResponse(void);
        Server getServer();
        Route  getRoute(Server &);
        void readContent(std::string const &filePath, HttpStatusCode code);
        std::string getStatusMessage(HttpStatusCode code);
        Route deepSearch(Server & server, std::string const & resource);
        void checkMethods(Route const & route);
        void checkRedirection(Route const & route);
        std::string getRequestedResource(std::string const & uri);
        Route findBestMatchInRoute(Route & route, std::string const & resource);
        Route findBestMatchInServer(Server & server, std::string const & resource);
        void handleGet(Server const &server, Route const &route);
        void handlePost(Server const &server, Route const &route);
        void handleDelete(Server const &server, Route const &route);
        std::string getFilePath(Server const & server, Route const & route);
        std::string tryFiles(Server const & server, Route const & route, std::string & root);
        void readBody(Route const & route);
        void processUrlEncodedBody(const std::string& body);
        void processMultipartFormDataBody(const std::string& body, Route const & route);
        void processFileUpload(std::istringstream& ss, const std::string& line, Route const & route);
        void processFormField(std::istringstream& ss, const std::string& line, std::map<std::string, std::string>& queryStrings);
};

void removeConsecutiveChars(std::string & s, char c);
std::pair<std::string, bool>    getMatchedPath(std::string serverRootPath, std::string path);
bool CheckIfInDirectory(std::string path);
bool CheckIfInFile(std::string path);