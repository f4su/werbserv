#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "Route.hpp"
#include "request_parser.hpp"
#include <netinet/in.h>

using	std::string;
using	std::vector;
using	std::map;

class	URI;

typedef map<int, string>	mapIntStr;
typedef map<int, URI>			mapIntUri;
typedef	vector<string>		vecStr;

void	trimTrailingSlashes(string & s);


class Server
{
private:
		static int			max;
    int             socket;
    int             port;
    uint32_t				host;
    size_t          clientMaxBodySize;
    bool            allowListing;
    vecStr          serverNames;
    string          root;
    vecStr          index;
		vector<int>			clients;
    mapIntUri				client_uri;
    mapIntStr       errorPages;
    vector<Route>   routes;

public:
    Server();
    ~Server();

    static int      getMax();
    int             getSocket() const;
		vector<int>     getClients() const;
    int             getPort() const;
    uint32_t				getHost() const;
		string					getHostStr()const;
    vecStr          getServerNames() const;
    size_t          getClientMaxBodySize() const;
    string          getRoot() const;
    vecStr          getIndex() const;
    mapIntStr       getErrorPages() const;
    bool            getAllowListing() const;
    vector<Route>		getRoutes() const;
    mapIntUri				&getClientUri();
    static void     setMax(int const &);
    void            setSocket(const int &);
    bool						setClients(const int&);
    void            setPort(const int &);
    bool						setHost(const string &);
    void            setServerNames(const vecStr&);
    void            setClientMaxBodySize(const size_t &);
    void            setRoot(const string &);
    void            setAllowListing(const bool &);
    void            setIndex(const vecStr&);
    void            setErrorPages(const mapIntStr&);
		void						setClientUri(const mapIntUri&);
    void            addRoute(Route route);
    void            print() const;
    void            fill(string const &, int &);

    typedef vector<Route>::iterator				iterator;

    iterator 				begin();
    iterator 				end();
    iterator 				find(const string& path);
		void						remove_client(const int &);

};
