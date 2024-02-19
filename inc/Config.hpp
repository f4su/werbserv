#pragma once

#include "idk.hpp"
#include <fstream>

class Route;
class Server;

enum state
{
    SERVER,
    ROUTE,
    NONE
};

class Config
{
private:
    static std::vector<Server> servers;
    Config();
    ~Config();

public:
    static void                         parse(std::string const &);
    static Server                       parseServer(std::ifstream &, std::string &, int &, std::stack<state> &);
    static Route                        parseRoute(std::ifstream &, std::string &, int &, std::stack<state> &);
    static std::string                  findRoute(std::string, int);
    static std::vector<Server>          getServers();
    typedef std::vector<Server>::iterator iterator;
    static iterator begin() { return servers.begin(); }
    static iterator end() { return servers.end(); }
    static iterator find(const std::string& host, int port)
    {
        for (iterator it = servers.begin(); it != servers.end(); ++it) {
            std::vector<std::string> sNames = it->getServerNames();
            if ((host == it->getHost() || std::find(sNames.begin(), sNames.end(), host) != sNames.end()) && it->getPort() == port) {
                return it;
            }
        }
        return servers.end(); 
    }
};
