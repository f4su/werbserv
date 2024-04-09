#include "../inc/Utils.hpp"
#include "../inc/status_codes.hpp"
#include  "../inc/Response.hpp"

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
    return (splited);
}

std::vector<std::string> ft_split_by_string(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        std::string token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + delimiter.length();
    }

    std::string lastToken = str.substr(start);
    tokens.push_back(lastToken);

    return tokens;
}

bool    mapErrorPages(std::map<int, std::string> & errorPages, std::string const & value)
{
    std::vector<std::string>    pages = ft_split(value, ", ");
    for (std::vector<std::string>::iterator it = pages.begin(); it != pages.end(); ++it)
    {
        std::vector<std::string>    page = ft_split(*it, ":");
        if (page.size() != 2)
            return (false);
        int code = std::atoi(page[0].c_str());
        if (code < 100 || code > 599)
            return (false);
        errorPages[code] = page[1];
    }
    if (pages.empty())
        return (false);
    return (true);
}

bool mapCgi(std::map<std::string, std::string> & cgi, std::string const & value)
{
    std::vector<std::string>    cgiVals = ft_split(value, ", ");
    for (std::vector<std::string>::iterator it = cgiVals.begin(); it != cgiVals.end(); ++it) {
        std::vector<std::string>    page = ft_split(*it, ":");
        if (page.size() != 2)
            return (false);
        cgi[page[0]] = page[1];
    }
    if (cgiVals.empty())
        return (false);
    return (true);
}

void trimTrailingSlashes(std::string & s)
{
    while (!s.empty() && s.back() == '/')
        s.pop_back();
}



std::vector<std::string> getFilesInDirectory(std::string & reqPath)
{
    std::vector<std::string> files;
    if (CheckInDirectory(reqPath) == false)
        throw ServerException(STATUS_404);
    
    DIR* dir = opendir(reqPath.c_str());
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string dirName = std::string(entry->d_name);
        if (dirName != "." && dirName !=  "..") {
            std::string path = reqPath + "/" + dirName;
            removeConsecutiveChars(path, '/');
            if (entry->d_type == DT_DIR)
                files.push_back(path + std::string("/"));
            else
                files.push_back(path);
        }
    }
    closedir(dir);
    return files;
}

std::string generateHtmlListing(const std::vector<std::string>& files)
{

    std::string html, fileName;
    html += "<!DOCTYPE html>\n<html>\n<head>\n<title>Directory Listing</title>\n</head>\n<body>\n";
    html += "<h1 style='padding-left:20px'>Directory Listing</h1>\n";
    html += "<ul>\n";
    for (size_t i = 0; i < files.size(); ++i) {
        html += "  <li style='font-size:20px'><a href='" + files[i] + "'>" + files[i] + "</a></li>\n";
    }
    html += "</ul>\n";
    html += "</body>\n</html>\n";

    return html;
}

bool isDirectory(std::string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return false;

    return S_ISDIR(stat_buf.st_mode);
}

bool isFile(std::string path)
{
    struct stat stat_buf;
    if (stat(path.c_str(), &stat_buf) != 0)
        return false;

    return S_ISREG(stat_buf.st_mode);
}

void removeFile(std::string path)
{
    if (std::remove(path.c_str()) != 0)
        throw ServerException(STATUS_500);	// send()
}

void removeDirectory(std::string path)
{
    DIR* dir = opendir(path.c_str());
    if (!dir)
        throw ServerException(STATUS_500);

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string dirName = std::string(entry->d_name);
        if (dirName != "." && dirName !=  "..")
        {
            std::string fullPath = path + "/" + dirName;
            removeConsecutiveChars(fullPath, '/');
            if (isFile(fullPath))
                removeFile(fullPath);
            else if (isDirectory(fullPath))
                removeDirectory(fullPath);
        }
    }
    closedir(dir);
}

void removeFileOrDirectory(std::string path)
{
    if (isFile(path))
    {
        removeFile(path);
        throw ServerException(STATUS_204);
    }
    else if (isDirectory(path))
    {
        removeDirectory(path);
        throw ServerException(STATUS_204);
    }
    else 
        throw ServerException(STATUS_404);
}
