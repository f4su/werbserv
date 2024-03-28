#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Route.hpp"
#include <netinet/in.h>

void	trimTrailingSlashes(std::string & s);

class Server
{
private:
		static int																	max;
    int                                         socket;
		std::vector<int>														clients;
    int                                         port;
    uint32_t																		host;
    std::string                                 host2;
    std::vector<std::string>                    serverNames;
    size_t                                      clientMaxBodySize;
    std::string                                 root;
    std::vector<std::string>                    index;
    std::map<int, std::string>                  errorPages;
    bool                                        allowListing;
    std::vector<Route>                          routes;

public:
    Server();
    ~Server();

    static int                                  getMax();
    int                                         getSocket() const;
		std::vector<int>                            getClients() const;
    int                                         getPort() const;
    uint32_t																		getHost() const;
    std::string                                 getHost2() const;
		std::string																	getHostStr()const;
    std::vector<std::string>                    getServerNames() const;
    size_t                                      getClientMaxBodySize() const;
    std::string                                 getRoot() const;
    std::vector<std::string>                    getIndex() const;
    std::map<int, std::string>                  getErrorPages() const;
    bool                                        getAllowListing() const;
    std::vector<Route>                          getRoutes() const;
    static void                                 setMax(int const &);
    void                                        setSocket(const int &);
    bool																				setClients(const int&);
    void                                        setPort(const int &);
    bool																				setHost(const std::string &);
    void                                        setServerNames(const std::vector<std::string>&);
    void                                        setClientMaxBodySize(const size_t &);
    void                                        setRoot(const std::string &);
    void                                        setAllowListing(const bool &);
    void                                        setIndex(const std::vector<std::string>&);
    void                                        setErrorPages(const std::map<int, std::string>&);
    void                                        addRoute(Route route);
    void                                        print() const;
    void                                        fill(std::string const &, int &);

    typedef std::vector<Route>::iterator				iterator;

    iterator 																		begin();
    iterator 																		end();
		void																				remove_client(const int &);

    iterator find(const std::string& path)
    {
        for (iterator it = routes.begin(); it != routes.end(); ++it)
        {
            std::string tmp = path;
            trimTrailingSlashes(tmp);
            if (it->getPath() == tmp)
                return it;
        }
        return routes.end();
    }


};
