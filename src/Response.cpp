#include "../inc/Response.hpp"
#include "../inc/headers.hpp"

#define	CRLF			"\r\n"

Response::Response(URI &rq)
{
    mime = Mime();
    isListing = false;
    request = &rq;
    //request = new URI();
    code = request->getStatusCode();
}

Response::~Response()
{
    //delete request;
}

void Response::checkRedirection(Route const & route)
{
    if (!route.getRedirect().empty())
    {
        headers["Location"] = route.getRedirect();
        //setStatus(STATUS_301);
        throw ServerException(STATUS_301);
    }
}

std::pair<string, bool>    getMatchedPath(string serverRootPath, string path)
{
    std::pair<string, bool> value;
    value.second = false;
    trimTrailingSlashes(serverRootPath);       
    if (path.find(serverRootPath) == 0)
    {
        if (path.length() == serverRootPath.length() || path[serverRootPath.length()] == '/')
        {
            value.first = path.substr(serverRootPath.length());
            value.second = true;
        } 
    }
    else
        value.first = path;
    return (value);
}

bool CheckInDirectory(string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return (false);
    return (S_ISDIR(stat_buf.st_mode));
}

bool CheckIfInFile(string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return (false);
    return (S_ISREG(stat_buf.st_mode));
}

Route Response::findBestMatchInRoute(Route & route, string const & resource)
{
    if (CheckInDirectory(route.getRoot() + resource))
    {
        if (resource.back() != '/') {
            if (request->getMethod() == 'd')
            {
                //setStatus(STATUS_409);
                throw ServerException(STATUS_409);
            }
            else
            {
                headers["Location"] = route.getPath() + resource + "/";
                //setStatus(STATUS_301);
                throw ServerException(STATUS_301);
            }
        }
        Route newRoute(route.getRoot() + resource, route.getPath() + resource, Route::DIRECTORY);
        newRoute.setAllowListing(route.getAllowListing());
        return (newRoute);
    }
    else if (CheckIfInFile(route.getRoot() + resource))
    {
        Route newRoute(route);
        newRoute.setPath(resource);
        newRoute.setRouteType(Route::FILE);
        return (newRoute);
    }
		std::cout << RED << "PACOOOO" << EOC << std::endl;
    //setStatus(STATUS_404);
    throw ServerException(STATUS_404);
}

Route Response::findBestMatchInServer(Server & server, string const & resource)
{ 
    std::cout << RED << "FIND BEST MATCH: root:" << server.getRoot() << " resource:" << resource << EOC << std::endl;
    if (CheckInDirectory(server.getRoot() + resource))
    {
        if (resource.back() != '/')
        {
            if (request->getMethod() == 'd')
            {
                //setStatus(STATUS_409);
                throw ServerException(STATUS_409);
            }
            else
            {
                headers["Location"] = resource + "/";
                //setStatus(STATUS_301);
                throw ServerException(STATUS_301);
            }
        }
        Route newRoute(server.getRoot() + resource, resource, Route::DIRECTORY);
        newRoute.setAllowListing(server.getAllowListing());
    		std::cout << RED << "Returning Route: root->" << newRoute.getRoot() << " path->" << newRoute.getPath() << EOC << std::endl;
        return (newRoute);
    }
    else if (CheckIfInFile(server.getRoot() + resource))
        return (Route(server.getRoot(), resource, Route::FILE));
    //setStatus(STATUS_404);
    throw ServerException(STATUS_404);
}

Route Response::deepSearch(Server & server, string const & resource)
{
    std::cout << RED << "DEEP SEARCH: " << resource << EOC << std::endl;
    vector<Route> routes = server.getRoutes();
    for (vector<Route>::iterator it = routes.begin(); it != routes.end(); ++it)
    {
        std::pair<string, bool>   matchedPath = getMatchedPath(it->getPath(), resource);
        if (matchedPath.second)
        {
            string newResource = "/" + matchedPath.first;
            removeConsecutiveChars(newResource, '/');
            Route route = findBestMatchInRoute(*it, newResource);
            checkRedirection(*it);
            return (route);
        }
    }
    return (findBestMatchInServer(server, resource)); 
}

Route Response::getRoute(Server & server)
{
    string resource = request->getPath();
    vector<Route>::iterator it = server.find(resource);

    std::cout << RED << "RESOURCE IS: " << resource << EOC << std::endl;
    if (it == server.end())
        return (deepSearch(server, resource));
    if (resource.back() != '/')
    {
        if (request->getMethod() == 'd')
        {
            //setStatus(STATUS_409);
            throw ServerException(STATUS_409);
        }
        headers["Location"] = request->getUri() + "/";
        //setStatus(STATUS_301);
        throw ServerException(STATUS_301);
    }
    checkRedirection(*it);
    return (*it);
}

Server Response::getServer()
{
    vector<Server>::iterator it = Config::find(request->getHost(), request->getPort());
    if (it != Config::end())
        if (request->getMethod() != 'g' && it->getClientMaxBodySize() != 0 && \
            request->getBody().size() > it->getClientMaxBodySize())
            {
                //setStatus(STATUS_413);
                throw ServerException(STATUS_413);
            }
        return *it;
    Server s = *(Config::begin());
    if (request->getMethod() != 'g' && s.getClientMaxBodySize() != 0 && \
            request->getBody().size() > s.getClientMaxBodySize())
            {
                //setStatus(STATUS_413);
                throw ServerException(STATUS_413);
            }
    return (s);
}

void Response::readContent(string const &filePath, string code)
{
    std::ifstream file(filePath);
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---readContent(string const &filePath, string code)" << EOC << std::endl;

    if (file.is_open())
    {
        headers["Content-Type"] = mime[Cgi::getFileExt(filePath)];
				std::stringstream buffer;
        string line;
        while (std::getline(file, line))
            buffer << line << std::endl;
        body = buffer.str();
        file.close();
    }
    else
    {
        code = code == STATUS_200 ? STATUS_404 : code;
        body = "<!DOCTYPE html><html><h1 align='center'>" + code + "</h1></html>";
    }
}

void Response::handleGet(Server &server, Route const & route)
{
    string filePath = getFilePath(server, route);
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---handleGet(Server &server, Route const & route)" << EOC << std::endl;
    if (isListing)
        return;
    removeConsecutiveChars(filePath, '/');
    if (!route.getCgi().empty())
    {
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        if (Cgiheaders.find("Content-Type") == Cgiheaders.end())
            headers["Content-Type"] = "text/html";
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readContent(filePath, STATUS_200);
}

string Response::tryFiles(Server const & server, Route const & route, string & root)
{
    if (route.getRouteType() == Route::DIRECTORY)
        root = route.getRoot();
    vector<string> indexes;
    if (route.getRouteType() == Route::FILE)
        indexes.push_back(route.getPath());
    else
        indexes = route.getIndex().empty() \
            ? (server.getIndex().empty() ? vector<string>() : server.getIndex()) \
            : route.getIndex();
    indexes.push_back("index.html");
    string filePath;
    for (vector<string>::iterator it = indexes.begin(); it != indexes.end(); it++)
    {
        filePath = root + "/" + *it;
        std::ifstream file(filePath);
        if (file.good())
        {
            file.close();
            return *it;
        }
        file.close();
    }
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---tryFiles(Server const & server, Route const & route, string & root)" << EOC << std::endl;
    if (route.getRouteType() != Route::FILE)
    {
        //setStatus(STATUS_403);
        throw ServerException(STATUS_403);
    }
    //setStatus(STATUS_404);
    throw ServerException(STATUS_404);
}

string Response::getFilePath(Server const & server, Route const & route)
{

    std::cout << RED << "GETTING FILE PATH--- " << EOC << std::endl;
    if (route.getRouteType() == Route::FILE)
    	std::cout << RED << "FILE PATH -- IS FILE" << EOC << std::endl;
      return route.getRoot() + "/" + route.getPath();
    try
    {
        string root, index;
        root = route.getRoot().empty() ? server.getRoot() : route.getRoot();
        index = tryFiles(server, route, root);
        return root + "/" + index;
    } catch (ServerException & e)
    {				//Revisar este status también
        if (request->getStatusCode() == STATUS_403 && (route.getAllowListing() || server.getAllowListing())  \
            && request->getMethod() != 'p')
        {
            if (request->getMethod() == 'd')
                return route.getRoot() + "/";
            vector<string> files = getFilesInDirectory(route.getRoot(), route.getPath());
            return (isListing = true, body = generateHtmlListing(files), "");
        }
        throw ServerException(request->getStatusCode());
    }
}

void Response::handleDelete(Server &server, Route const & route)
{
    string filePath = getFilePath(server, route);
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---handleDelete(Server &server, Route const & route)" << EOC << std::endl;
    removeConsecutiveChars(filePath, '/');
    if (!route.getCgi().empty())
    {
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    removeFileOrDirectory(filePath);
}

void Response::processUrlEncodedBody(const string& body)
{
    std::map<string, string> queryStrings;
    vector<string> params = ft_split(body, "&");

    for (vector<string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        vector<string> param = ft_split(*it, "=");
        if (param.size() == 2)
            queryStrings[param[0]] = param[1];
    }
}

void Response::processFileUpload(std::istringstream& ss, const string& line, Route const & route)
{
    int len = line.find("\"", line.find("filename") + 10) - line.find("filename") - 10;
    string filename = route.getUploadDir() + "/" + line.substr(line.find("filename") + 10, len);
    std::ofstream file(filename.c_str());
    string content;
    std::getline(ss, content);
    std::getline(ss, content);
    while (std::getline(ss, content))
        file << content << "\n";
}

void Response::processFormField(std::istringstream& ss, const string& line, std::map<string, string>& queryStrings)
{
    int len = line.find("\"", line.find("name") + 6) - line.find("name") - 6;
    string name = line.substr(line.find("name") + 6, len);
    string content;
    std::getline(ss, content);
    std::getline(ss, content);
    std::getline(ss, content, '\0');
    queryStrings[name] = content;
}

void Response::processMultipartFormDataBody(const string& body, Route const & route)
{
    std::map<string, string> queryStrings;
    string boundary = request->getBoundary();
    vector<string> params = ft_split(body, "--" + boundary);

    for (vector<string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        std::istringstream ss(*it);
        string line;
        std::getline(ss, line);

        if (line.find("filename") != string::npos)
            processFileUpload(ss, line, route);
        else if (line.find("name") != string::npos)
            processFormField(ss, line, queryStrings);
    }
}

void Response::readBody(Route const & route)
{
    const string& contentType = request->getContentType();
    const string& body = request->getBody();
    
    if (contentType == "application/x-www-form-urlencoded")
        processUrlEncodedBody(body);
    else if (contentType == "multipart/form-data")
        processMultipartFormDataBody(body, route);
}

void Response::handlePost(Server &server, Route const & route)
{
    if (!route.getCgi().empty())
    {
        string filePath = getFilePath(server, route);
         std::cout << CYA << "FILEPATH IS : " << filePath << " <---handlePost(Server &server, Route const & route)" << EOC << std::endl;
        removeConsecutiveChars(filePath, '/');
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readBody(route);
    //setStatus(STATUS_201);
    throw ServerException(STATUS_201);
}

string getDateGMT()
{
    std::time_t t = std::time(0);
    std::tm tm = *std::gmtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
    return (ss.str());
}

string Response::getResponse()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << request->getStatusCode() << CRLF;
    ss << "Server: webserv/1.0" << CRLF;
    ss << "Date: " << getDateGMT() << CRLF;
    ss << "Content-Length: " << toString(body.size()) << CRLF;
    for (std::map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
        ss << it->first << ": " << it->second << CRLF;
    ss << CRLF;
    ss << body;

    return (ss.str());
}

void    Response::setStatus(string codestr)
{
    if (code == "")
        code = codestr;
}

void Response::handleResponse(Server &server)
{
    try
    {
        std::cout << RED << "START TO HANDLE PACOO"<< EOC << std::endl;
				headers["Content-Type"] = "text/html";
        Route route = getRoute(server);
        if (request->getMethod() == 'g'){
            std::cout << RED << "IN GET "<< EOC << std::endl;
            handleGet(server, route);
        }
        else if (request->getMethod() == 'p'){
            std::cout << RED << "IN POST "<< EOC << std::endl;
            handlePost(server, route);
        }
        else if (request->getMethod() == 'd'){ 
            std::cout << RED << "IN POST "<< EOC << std::endl;
            handleDelete(server, route);
        }
        else {
            std::cout << RED << "IN EXCEPT"<< EOC << std::endl;
            //setStatus(STATUS_501);
            throw ServerException(STATUS_501);			//TO DO: Todo lo que sean throws, convertirlos en send
		}
    }
    catch (ServerException & e)
    {
        std::cout << CYA << "MSGGGGGGGGGGGG>" << e.what() << EOC << std::endl;
       // std::cout << CYA << "///////////////////////CODE IS 1 : " << code << EOC << std::endl;
        //code = request->getStatusCode();
       // std::cout << CYA << "///////////////////////CODE IS 1 : " << code << EOC << std::endl;
        int c;
        std::string strnb = e.what();
        std::stringstream    ss(strnb.substr(0, 3));
        ss >> c;
        std::cout << CYA << "///////////////////////1st C IS 1: " << c << EOC << std::endl;
        //std::cout << CYA << "///////////////////////1st CODE IS 1: " << code << EOC << std::endl;
        std::cout << CYA << "///////////////////////1st FILEPATH IS 1: " << server.getErrorPages()[c] << EOC << std::endl;
        readContent(server.getErrorPages()[c], e.what());
    }
    catch (std::exception & e)
    {
        std::cout << CYA << "///////////////////////CODE IS 2 : " << code << EOC << std::endl;
        code = STATUS_500;
        std::cout << CYA << "///////////////////////CODE IS 2 : " << code << EOC << std::endl;
        int c = 500;
        std::cout << CYA << "///////////////////////FILEPATH IS 2 : " << server.getErrorPages()[c] << EOC << std::endl;
        readContent(server.getErrorPages()[c], code);
    }
}
