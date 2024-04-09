#include "../inc/Response.hpp"

bool CheckInDirectory(string &path)
{
    if (path.size() == 1 && path[0] == '.')
        path = path + "/";
		else if (path.size() && path[0] == '/')
        path = "." + path;
		else if (path.size() && path[0] != '/' && path[0] != '.')
        path = "./" + path;

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
            request->getBody().size() > it->getClientMaxBodySize()){
                throw ServerException(STATUS_413);
            }
        return *it;
    Server s = *(Config::begin());
    if (request->getMethod() != 'g' && s.getClientMaxBodySize() != 0 && \
            request->getBody().size() > s.getClientMaxBodySize()){
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
