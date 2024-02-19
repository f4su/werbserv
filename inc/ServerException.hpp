#pragma once

#include <exception>
#include <string>
#include <iostream>
#include <sstream>

typedef enum e_HttpStatusCode {
    OK = 200,
    MovedPermanently = 301,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    Conflict = 409,
    MethodNotAllowed = 405,
    RequestURITooLong = 414,
    RequestEntityTooLarge = 413,
    NotImplemented = 501,
    ServerError = 500,
    NoContent = 204,
    BadGateway = 502,
    Created = 201,
} HttpStatusCode;

class ServerException : public std::exception
{
private:
    std::string msg;
    HttpStatusCode code;
public:
    ServerException(std::string const &msg) throw();
    ServerException(std::string const &msg, int line) throw();
    ServerException(HttpStatusCode code) throw();
    virtual ~ServerException() throw();
    virtual const char *what() const throw();
    virtual HttpStatusCode getCode() const throw();
};