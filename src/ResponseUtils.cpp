#include "../inc/Response.hpp"

Route Response::findBestMatchInRoute(Route & route, string const & resource)
{
    if (CheckInDirectory(resource))
    {
        std::cerr << MAG << "-----PATH IS A DIRECTORY-----" << EOC << std::endl;
        if (resource.back() != '/') {
            if (request->getMethod() == 'd')
            {
                //setStatus(STATUS_409);
                throw ServerException(STATUS_409);
            }
            else
            {
                headers["Location"] = route.getPath() + resource + "/";
                //setStatus(STATUS_307);
                throw ServerException(STATUS_307);
            }
        }
        Route newRoute(resource, route.getPath() + resource, Route::DIRECTORY);
        newRoute.setAllowListing(route.getAllowListing());
        return (newRoute);
    }
    else if (CheckIfInFile(resource))
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
    
    if (CheckInDirectory(resource))
    {
        std::cerr << MAG << "-----PATH IS A DIRECTORY-----" << EOC << std::endl;
        if (resource.back() != '/')
        {
            if (request->getMethod() == 'd')
            {
                //setStatus(STATUS_409);
                throw ServerException(STATUS_409);
            }
            else
            {
                headers["Location"] = resource;
                //setStatus(STATUS_307);
                throw ServerException(STATUS_307);
            }
        }
        Route newRoute(resource, resource, Route::DIRECTORY);
        newRoute.setAllowListing(server.getAllowListing());
    	std::cout << RED << "Returning Route: root->" << newRoute.getRoot() << " path->" << newRoute.getPath() << EOC << std::endl;
        return (newRoute);
    }
    else if (CheckIfInFile(resource))
        return (Route(server.getRoot(), resource, Route::FILE));
    //setStatus(STATUS_404);
    throw ServerException(STATUS_404);
}

bool CheckInDirectory(string path)
{
    if (path.size() && path.front() != '/')
        path = getCurrentDirectory() + "/" + path;
    else if (path.size() )
        path = getCurrentDirectory() + path;

    std::cerr << MAG << "QUE PATH ES ESTE -----------> " << path << EOC << std::endl;
    DIR* dir = opendir(path.c_str());
    if (dir != NULL) {
        closedir(dir);
        return true;
    } else {
        return false;
    }
}

bool CheckIfInFile(string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return (false);
    return (S_ISREG(stat_buf.st_mode));
}

std::string getCurrentDirectory() {
    char path[FILENAME_MAX];
    if (getcwd(path, sizeof(path)) != NULL) {
        return std::string(path);
    } else {
        // Error occurred while getting current directory
        return "";
    }
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
void Response::checkRedirection(Route const & route)
{
    std::cout << CYA <<"CHECKING REDIRECTION ON PATH--------> " << route.getPath() << EOC << std::endl;
    if (!route.getRedirect().empty())
    {
        headers["Location"] = route.getRedirect();
        throw ServerException(STATUS_307);
    }
}


void removeConsecutiveChars(std::string & s, char c)
{
    std::string result;
    std::string::iterator it;
    
    for (it = s.begin(); it != s.end(); ++it)
        if (result.empty() || *it != c || result.back() != c)
            result.push_back(*it);
    s = result;
}