#pragma once

#include <iostream>
#include <vector>
#include <map>

class Route
{
public:
    enum RouteType {
        FILE,
        DIRECTORY,
        OTHER
    };
    
    Route();
    Route(std::string const & path);
    Route(std::string const & root, std::string const & path, RouteType routeType);
    ~Route();

    std::string                                 getPath() const;
    std::string                                 getRoot() const;
    std::vector<std::string>                    getIndex() const;
    std::string                                 getRedirect() const;
    std::vector<std::string>                    getMethods() const;
    std::string                                 getUploadDir() const;
    bool                                        getAllowListing() const;
    bool                                        getAllowListingSet() const;
    std::map<int, std::string>                  getErrorPages() const;
    std::map<std::string, std::string>          getCgi() const;
    void                                        setPath(const std::string& path);
    void                                        setRoot(const std::string& root);
    void                                        setIndex(const std::vector<std::string>& index);
    void                                        setRedirect(const std::string& redirect);
    void                                        setMethods(const std::vector<std::string>& methods);
    void                                        setUploadDir(const std::string& uploadDir);
    void                                        setAllowListing(const bool& allowListing);
    void                                        setAllowListingSet(const bool& allowListingSet);
    void                                        setErrorPages(const std::map<int, std::string>&);

    RouteType getRouteType() const{
        return (routeType);
    }

    void setRouteType(RouteType type) {
        routeType = type;
    }

    void                        print() const;
    void                        fill(std::string const &line, int &lineNb);
private:
    std::string                             path;
    std::string                             root;
    std::vector<std::string>                index;
    std::string                             redirect;
    std::vector<std::string>                methods;
    std::string                             uploadDir;
    std::map<int, std::string>              errorPages;
    bool                                    allowListing;
    bool                                    allowListingSet;
    std::map<std::string, std::string>      cgi;
    RouteType                               routeType;
};