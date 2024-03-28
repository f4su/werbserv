#include "../inc/Response.hpp"

//Response(std::string const &buffer){
Response::Response()
{
    mime = Mime();
    isListing = false;
    request = new URI();
    code = request->getStatusCode();
    handleResponse();
}

Response::~Response()
{
    delete request;
}

void Response::checkRedirection(Route const & route)
{
    if (!route.getRedirect().empty())
    {
        headers["Location"] = route.getRedirect();
        throw ServerException(MovedPermanently);
    }
}

void Response::checkMethods(Route const & route)
{
    std::vector<std::string> methods = route.getMethods();
    if (!methods.empty() && std::find(methods.begin(), methods.end(), request->getMethod2()) == methods.end())
        throw ServerException(MethodNotAllowed);
}

std::pair<std::string, bool>    getMatchedPath(std::string serverRootPath, std::string path)
{
    std::pair<std::string, bool> value;
    value.second = false;
    trimTrailingSlashes(serverRootPath);       
    if (path.find(serverRootPath) == 0) {
        if (path.length() == serverRootPath.length() || path[serverRootPath.length()] == '/') {
            value.first = path.substr(serverRootPath.length());
            value.second = true;
        } 
    }
    else
        value.first = path;
    return (value);
}

bool CheckInDirectory(std::string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return (false);
    return (S_ISDIR(stat_buf.st_mode));
}

bool CheckIfInFile(std::string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return (false);
    return (S_ISREG(stat_buf.st_mode));
}

Route Response::findBestMatchInRoute(Route & route, std::string const & resource)
{
    if (CheckInDirectory(route.getRoot() + resource))
    {
        if (resource.back() != '/') {
            if (request->getMethod2() == "DELETE")
                throw ServerException(Conflict);
            else
            {
                headers["Location"] = route.getPath() + resource + "/";
                throw ServerException(MovedPermanently);
            }
        }
        Route newRoute(route.getRoot() + resource, route.getPath() + resource, Route::DIRECTORY);
        newRoute.setAllowListing(route.getAllowListing());
        return newRoute;
    }
    else if (CheckIfInFile(route.getRoot() + resource))
    {
        Route newRoute(route);
        newRoute.setPath(resource);
        newRoute.setRouteType(Route::FILE);
        return (newRoute);
    }
    throw ServerException(NotFound);
}

Route Response::findBestMatchInServer(Server & server, std::string const & resource)
{
    if (CheckInDirectory(server.getRoot() + resource))
    {
        if (resource.back() != '/')
        { // Check if resource ends with a slash (if not, redirect to resource/
            if (request->getMethod2() == "DELETE")
                throw ServerException(Conflict);
            else
            {
                headers["Location"] = resource + "/";
                throw ServerException(MovedPermanently);
            }
        }
        Route newRoute(server.getRoot() + resource, resource, Route::DIRECTORY);
        newRoute.setAllowListing(server.getAllowListing());
        return (newRoute);
    }
    else if (CheckIfInFile(server.getRoot() + resource)) // Check if resource is a file
        return (Route(server.getRoot(), resource, Route::FILE));
    throw ServerException(NotFound);
}

Route Response::deepSearch(Server & server, std::string const & resource)
{
    std::vector<Route> routes = server.getRoutes();
    for (std::vector<Route>::iterator it = routes.begin(); it != routes.end(); it++)
    {
        std::pair<std::string, bool>   matchedPath = getMatchedPath(it->getPath(), resource);
        if (matchedPath.second)
        {
            std::string newResource = "/" + matchedPath.first;
            removeConsecutiveChars(newResource, '/');
            Route route = findBestMatchInRoute(*it, newResource);
            checkRedirection(*it);
            checkMethods(*it);
            return (route);
        }
    }
    return (findBestMatchInServer(server, resource)); 
}

std::string Response::getRequestedResource(std::string const & uri)
{
    std::string resource = uri;
    size_t pos = resource.find('?');
    if (pos != std::string::npos)
        resource = resource.substr(0, pos);
    return (resource);
}

Route Response::getRoute(Server & server)
{
    std::string resource = getRequestedResource(request->getUri());
    std::vector<Route>::iterator it = server.find(resource);

    if (it == server.end())
        return (deepSearch(server, resource));
    if (resource.back() != '/')
    {
        if (request->getMethod2() == "DELETE")
            throw ServerException(Conflict);
        headers["Location"] = request->getUri() + "/";
        throw ServerException(MovedPermanently);
    }
    checkRedirection(*it);//checkkeame las redirecciones y los metodos 
    checkMethods(*it);
    if (request->getMethod2() != "GET" && server.getClientMaxBodySize() != 0 && \
            request->getBody().size() > server.getClientMaxBodySize())
        throw ServerException(RequestEntityTooLarge);
    return (*it);
}

Server Response::getServer()
{
    std::vector<Server>::iterator it = Config::find(request->getHost(), request->getPort());
    if (it != Config::end())
        if (request->getMethod2() != "GET" && it->getClientMaxBodySize() != 0 && \
            request->getBody().size() > it->getClientMaxBodySize())
                throw ServerException(RequestEntityTooLarge);
        return *it;
    Server s = *(Config::begin());
    if (request->getMethod2() != "GET" && s.getClientMaxBodySize() != 0 && \
            request->getBody().size() > s.getClientMaxBodySize())
                throw ServerException(RequestEntityTooLarge);
    return (s);
}

std::string Response::getStatusMessage(HttpStatusCode code)
{
    switch (code)
    {
        case OK: return "OK";
        case BadRequest: return "Bad Request";
        case Unauthorized: return "Unauthorized";
        case Forbidden: return "Forbidden";
        case NotFound: return "Not Found";
        case Conflict: return "Conflict";
        case MethodNotAllowed: return "Method Not Allowed";
        case RequestURITooLong: return "Request-URI Too Long";
        case RequestEntityTooLarge: return "Request Entity Too Large";
        case NotImplemented: return "Not Implemented";
        case NoContent: return "No Content";
        case ServerError: return "Internal Server Error";
        case BadGateway: return "Bad Gateway";
        case Created: return "Created";
        default: return "Unknown";
    }
}

void Response::readContent(std::string const &filePath, HttpStatusCode code)
{
    std::ifstream file(filePath);

    if (file.is_open())
    {
        headers["Content-Type"] = mime[Cgi::getFileExt(filePath)];
        std::stringstream buffer;
        std::string line;
        while (std::getline(file, line))
            buffer << line << std::endl;
        body = buffer.str();
        file.close();
    }
    else
    {
        code = code == OK ? NotFound : code;
        body = "<!DOCTYPE html><html><h1 align='center'>" + toString(code) + " " + getStatusMessage(code) + "</h1></html>";
    }
}

void Response::handleGet(Server const & server, Route const & route)
{
    std::string filePath = getFilePath(server, route);
    if (isListing)
        return;
    removeConsecutiveChars(filePath, '/');
    if (!route.getCgi().empty())
    {
        Cgi cgi(route, filePath, *request);
        std::map<std::string, std::string> Cgiheaders = cgi.getResponseHeaders();
        if (Cgiheaders.find("Content-type") == Cgiheaders.end())
            headers["Content-Type"] = "text/html";
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readContent(filePath, OK);
}

std::string Response::tryFiles(Server const & server, Route const & route, std::string & root)
{
    if (route.getRouteType() == Route::DIRECTORY)
        root = route.getRoot();
    std::vector<std::string> indexes;
    if (route.getRouteType() == Route::FILE)
        indexes.push_back(route.getPath());
    else
        indexes = route.getIndex().empty() \
            ? (server.getIndex().empty() ? std::vector<std::string>() : server.getIndex()) \
            : route.getIndex();
    indexes.push_back("index.html");
    std::string filePath;
    for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
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
    if (route.getRouteType() != Route::FILE)
        throw ServerException(Forbidden);
    throw ServerException(NotFound);
}

std::string Response::getFilePath(Server const & server, Route const & route)
{
    if (route.getRouteType() == Route::FILE)
        return route.getRoot() + "/" + route.getPath();
    
    try
    {
        std::string root, index;
        root = route.getRoot().empty() ? server.getRoot() : route.getRoot();
        index = tryFiles(server, route, root);
        return root + "/" + index;
    } catch (ServerException & e)
    {
        if (e.getCode() == Forbidden && (route.getAllowListing() || server.getAllowListing())  \
            && request->getMethod2() != "POST")
        {
            if (request->getMethod2() == "DELETE")
                return route.getRoot() + "/";
            std::vector<std::string> files = getFilesInDirectory(route.getRoot(), route.getPath());
            return (isListing = true, body = generateHtmlListing(files), "");
        }
        throw ServerException(e.getCode());
    }
}

void Response::handleDelete(Server const & server, Route const & route)
{
    std::string filePath = getFilePath(server, route);
    removeConsecutiveChars(filePath, '/');
    if (!route.getCgi().empty())
    {
        Cgi cgi(route, filePath, *request);
        std::map<std::string, std::string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    removeFileOrDirectory(filePath);
}

void Response::processUrlEncodedBody(const std::string& body)
{
    std::map<std::string, std::string> queryStrings;
    std::vector<std::string> params = ft_split(body, "&");

    for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        std::vector<std::string> param = ft_split(*it, "=");
        if (param.size() == 2)
            queryStrings[param[0]] = param[1];
    }
}

void Response::processFileUpload(std::istringstream& ss, const std::string& line, Route const & route)
{
    int len = line.find("\"", line.find("filename") + 10) - line.find("filename") - 10;
    std::string filename = route.getUploadDir() + "/" + line.substr(line.find("filename") + 10, len);
    std::ofstream file(filename.c_str());
    std::string content;
    std::getline(ss, content);
    std::getline(ss, content);
    while (std::getline(ss, content))
        file << content << "\n";
}

void Response::processFormField(std::istringstream& ss, const std::string& line, std::map<std::string, std::string>& queryStrings)
{
    int len = line.find("\"", line.find("name") + 6) - line.find("name") - 6;
    std::string name = line.substr(line.find("name") + 6, len);
    std::string content;
    std::getline(ss, content);
    std::getline(ss, content);
    std::getline(ss, content, '\0');
    queryStrings[name] = content;
}
void Response::processMultipartFormDataBody(const std::string& body, Route const & route)
{
    std::map<std::string, std::string> queryStrings;
    std::string boundary = request->getBoundary();
    std::vector<std::string> params = ft_split(body, "--" + boundary);

    for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        std::istringstream ss(*it);
        std::string line;
        std::getline(ss, line);

        if (line.find("filename") != std::string::npos)
            processFileUpload(ss, line, route);
        else if (line.find("name") != std::string::npos)
            processFormField(ss, line, queryStrings);
    }
}

void Response::readBody(Route const & route)
{
    const std::string& contentType = request->getContentType();
    const std::string& body = request->getBody();
    
    if (contentType == "application/x-www-form-urlencoded")
        processUrlEncodedBody(body);
    else if (contentType == "multipart/form-data")
        processMultipartFormDataBody(body, route);
}

void Response::handlePost(Server const & server, Route const & route)
{
    if (!route.getCgi().empty())
    {
        std::string filePath = getFilePath(server, route);
        removeConsecutiveChars(filePath, '/');
        Cgi cgi(route, filePath, *request);
        std::map<std::string, std::string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readBody(route);
    throw ServerException(Created);
}

void Response::handleResponse(void)
{
    Server  server;

    try
    {
        server = getServer();
        if (code != OK)
            throw ServerException(code);
        Route route = getRoute(server);
        if (request->getMethod2() == "GET")
            handleGet(server, route);
        else if (request->getMethod2() == "POST")
            handlePost(server, route);
        else if (request->getMethod2() == "DELETE")
           handleDelete(server, route);
        else
            throw ServerException(NotImplemented);
    }
    catch (ServerException & e)
    {
        code = e.getCode();
        readContent(server.getErrorPages()[code], code);
    }
    catch (std::exception & e)
    {
        code = ServerError;
        readContent(server.getErrorPages()[code], code);
    }
}