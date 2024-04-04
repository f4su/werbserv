#include "../inc/ServerException.hpp"

ServerException::ServerException(std::string const &msg) throw() : msg(msg)
{
}

ServerException::ServerException(std::string const &msg, int line) throw() : msg(msg)
{
    std::stringstream ss;
    ss << " at line " << line;
    this->msg += ss.str();
}

ServerException::~ServerException() throw()
{
}

const char *ServerException::what() const throw()
{
    return (this->msg.c_str());
}
