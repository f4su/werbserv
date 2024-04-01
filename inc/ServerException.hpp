#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <sstream>

class ServerException : public std::exception
{
private:
    std::string msg;
public:
    ServerException(std::string const &msg) throw();
    ServerException(std::string const &msg, int line) throw();
    virtual ~ServerException() throw();
    virtual const char *what() const throw();
};
