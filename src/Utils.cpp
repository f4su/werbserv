#include "../inc/Utils.hpp"

void trim(std::string & s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos)
        s.clear();
    else
        s = s.substr(start, end - start + 1);
}
std::vector<std::string> ft_split(const std::string &s, const std::string &delimiters)
{
    std::vector<std::string> splited;
    size_t i = 0;

    while (i < s.length())
    {
        i = s.find_first_not_of(delimiters, i);
        if (i == std::string::npos)
            break;
        size_t wordEnd = s.find_first_of(delimiters, i);
        std::string word = s.substr(i, wordEnd - i);
        trim(word);
        splited.push_back(word);
        
        i = wordEnd;
    }
    return splited;
}

bool    mapErrorPages(std::map<int, std::string> & errorPages, std::string const & value)
{
    std::vector<std::string>    pages = ft_split(value, ", ");
    for (std::vector<std::string>::iterator it = pages.begin(); it != pages.end(); ++it) {
        std::vector<std::string>    page = ft_split(*it, ":");
        if (page.size() != 2)
            return false;
        int code = std::atoi(page[0].c_str());
        if (code < 100 || code > 599)
            return false;
        errorPages[code] = page[1];
    }
    if (pages.empty())
        return false;
    return true;
}

bool mapCgi(std::map<std::string, std::string> & cgi, std::string const & value)
{
    std::vector<std::string>    cgiVals = ft_split(value, ", ");
    for (std::vector<std::string>::iterator it = cgiVals.begin(); it != cgiVals.end(); ++it) {
        std::vector<std::string>    page = ft_split(*it, ":");
        if (page.size() != 2)
            return false;
        cgi[page[0]] = page[1];
    }
    if (cgiVals.empty())
        return false;
    return true;
}

void trimTrailingSlashes(std::string & s)
{
    while (!s.empty() && s.back() == '/')
        s.pop_back();
}
