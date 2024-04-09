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
        URI 											*request;
        string										code;
        Mime 											mime;
        std::map<string, string>	headers;
        string										body;
        bool											isListing;
    		mapIntStr									errorPages;
    		string										uploadDir;

    public:
        //Response(string const &buffer);
        Response(URI &rq);
        ~Response();

				//Getters & Setter
        Server 	getServer();
        string	tryFiles(Server const & server, Route const & route, string & root);

        void		readContent(string const &filePath, string code);
        void 		checkRedirection(Route const & route);

        void 		handleGet(Server &server, Route const & route);
        void 		handlePost(Server &server, Route const & route);
				void 		handleDelete(Server &server);

        void 		handleResponse(Server &server, Route &route);

        void 		readBody(Route const & route);
        void 		processUrlEncodedBody(const string& body);
        void 		processMultipartFormDataBody(const string& body, Route const & route);
        void 		processFileUpload(std::istringstream& ss, const string& name);
        void 		processFormField(std::istringstream& ss, const string& name, std::map<string, string>& queryStrings);
        std::string getResponse();
        void    setStatus(std::string codestr);
				std::string	resolveFileName();
};

void 											removeConsecutiveChars(string & s, char c);
std::pair<string, bool>   getMatchedPath(string serverRootPath, string path);
bool 											CheckIfInDirectory(string path);
bool 											CheckIfInFile(string path);
bool                      CheckInDirectory(string &path);
std::string               getCurrentDirectory();
string										getDateGMT();
string										add_error_page_response(string &filePath, string &code);
