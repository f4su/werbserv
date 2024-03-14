#pragma once

#include	<unistd.h>
#include	<map>
#include	<vector>
#include <iostream>
#include <sstream>

template <typename Container>
void printContainer(const Container& c)
{
    typename Container::const_iterator it;
    for (it = c.begin(); it != c.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

template <typename Container>
void printMap(const Container& c)
{
    typename Container::const_iterator it;
    for (it = c.begin(); it != c.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << std::endl;    
    }
    std::cout << std::endl;
}

template <typename T>
std::string toString(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

void                        trim(std::string &);
std::vector<std::string>    ft_split(const std::string &, const std::string &);
bool                        mapErrorPages(std::map<int, std::string> & errorPages, std::string const & value);
bool                        mapCgi(std::map<std::string, std::string> & cgi, std::string const & value);
void                        trimTrailingSlashes(std::string & s);
