#pragma once

#include "idk.hpp"

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

void                        trim(std::string &);
std::vector<std::string>    ft_split(const std::string &, const std::string &);
bool                        mapErrorPages(std::map<int, std::string> & errorPages, std::string const & value);
bool                        mapCgi(std::map<std::string, std::string> & cgi, std::string const & value);
void                        trimTrailingSlashes(std::string & s);