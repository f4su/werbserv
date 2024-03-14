#pragma once

#include <fstream>
#include "Server.hpp"

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
    static void                         		parse(std::string const &);
    static Server                       		parseServer(std::ifstream &, std::string &, int &, std::stack<state> &);
    static Route                        		parseRoute(std::ifstream &, std::string &, int &, std::stack<state> &);
    static std::string                  		findRoute(std::string, int);
    static std::vector<Server>          		getServers();
    typedef std::vector<Server>::iterator 	iterator;
    static iterator 												begin();
    static iterator 												end();
};
