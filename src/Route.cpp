#include "../inc/Route.hpp"
#include "../inc/ServerException.hpp"
#include "../inc/Utils.hpp"

Route::Route() : path(""), root(""), index(), redirect(), methods(), uploadDir(""), errorPages(), allowListing(0), cgi(), routeType(OTHER)
{
};

Route::Route(std::string const &path) : path(path), root(""), index(), redirect(), methods(), uploadDir(""), errorPages(), allowListing(0), cgi(), routeType(OTHER)
{
};

Route::Route(std::string const &root, std::string const &path, RouteType routeType) : path(path), root(root), index(), redirect(), methods(), uploadDir(""), errorPages(), allowListing(0), cgi(), routeType(routeType)
{
};

Route::~Route()
{
};

std::string Route::getPath() const
{
    return path;
}
std::string Route::getRoot() const
{
    return root;
}
std::vector<std::string> Route::getIndex() const
{
    return index;
}
std::string Route::getRedirect() const
{
    return redirect;
}
std::vector<std::string> Route::getMethods() const
{
    return methods;
}
std::string Route::getUploadDir() const
{
    return uploadDir;
}
bool Route::getAllowListing() const
{
    return allowListing;
}
std::map<int, std::string> Route::getErrorPages() const { return errorPages; }
std::map<std::string, std::string> Route::getCgi() const { return cgi; }


void Route::setPath(const std::string &path)
{
    this->path = path;
}
void Route::setRoot(const std::string &root)
{
    this->root = root;
}
void Route::setIndex(const std::vector<std::string> &index)
{
    this->index = index;
}
void Route::setRedirect(const std::string &redirect)
{
    this->redirect = redirect;
}
void Route::setMethods(const std::vector<std::string> &methods)
{
    this->methods = methods;
}
void Route::setUploadDir(const std::string &uploadDir)
{
    this->uploadDir = uploadDir;
}
void Route::setAllowListing(const bool &allowListing)
{
    this->allowListing = allowListing;
}
void    Route::setErrorPages(const std::map<int, std::string> & errorPages) { this->errorPages = errorPages; }

void Route::print() const
{
    std::cout << "\t***********" << std::endl;
    if (!path.empty())
        std::cout << "\tpath: " << path << std::endl;
    if (!root.empty())
        std::cout << "\troot: " << root << std::endl;
    if (!index.empty())
    {
        std::cout << "\tindex: ";
        printContainer(index);
    }
    if (!redirect.empty())
        std::cout << "\tredirect: " << redirect << std::endl;
    if (!methods.empty())
    {
        std::cout << "\tmethods: ";
        printContainer(methods);
    }
    if (!uploadDir.empty())
        std::cout << "\tuploadDir: " << uploadDir << std::endl;
    if (allowListing)
        std::cout << "\tallowListing: " << allowListing << std::endl;
    if (!errorPages.empty()) {
        std::cout << "\terror_pages: ";
        printMap(errorPages);
    }
    if (!cgi.empty()) {
        std::cout << "\tcgi: ";
        printMap(cgi);
    }
    std::cout << "\tRouteType: " << (routeType == 0 ? "FILE" : (routeType == 1 ? "DIRECTORY" : "OTHER")) << std::endl;
}

void Route::fill(std::string const &line, int &lineNb)
{
    std::vector<std::string> split;
    std::string option, value;

    split = ft_split(line, "=");

    if (split.size() != 2)
        throw ServerException("Invalid route line", lineNb);
    option = split[0];
    value = split[1];
    if (value.empty() || option.empty())
        throw ServerException("Invalid route line", lineNb);
    if (option == "path" && path.empty())
    {
        setPath(value);
    }
    else if (option == "root" && root.empty())
    {
        setRoot(value);
    }
    else if (option == "index" && index.empty())
    {
        std::vector<std::string> index = ft_split(value, ", ");
        if (index.empty())
            throw ServerException("Invalid route line", lineNb);
        setIndex(index);
    }
    else if (option == "redirect" && redirect.empty())
    {
        setRedirect(value);
    }
    else if (option == "methods" && methods.empty())
    {
        std::vector<std::string> methods = ft_split(value, ", ");
        if (methods.empty())
            throw ServerException("Invalid route line", lineNb);
        setMethods(methods);
    }
    else if (option == "upload_dir" && uploadDir.empty())
    {
        setUploadDir(value);
    }
    else if (option == "allow_listing" && allowListing == 0)
    {
        if (value == "on" || value == "1" || value == "true")
            setAllowListing(true);
        else if (value == "off" || value == "0" || value == "false")
            setAllowListing(false);
        else
            throw ServerException("Invalid route line", lineNb);
    }
    else if (option == "error_pages" && errorPages.empty()) {
        if (!mapErrorPages(errorPages, value))
            throw ServerException("Invalid server line", lineNb);
    }
    else if (option == "cgi" && cgi.empty())
    {
        if (!mapCgi(cgi, value))
            throw ServerException("Invalid server line", lineNb);
    }
    else
        throw ServerException("Invalid server option", lineNb);
}
