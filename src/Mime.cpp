#include "../inc/Mime.hpp"
#include "../inc/Utils.hpp"


Mime::Mime()
{
    std::ifstream file("./mime.txt");
    std::string line;
    std::vector<std::string> pairs;
    
    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line[0] == '#')
                continue;
            
            pairs = ft_split(line, "|");
            if (pairs.size() != 2)
                continue;
            std::vector<std::string> exts = ft_split(pairs[0], ",");
            mime[exts] = pairs[1];
            pairs.clear();
        }
        file.close();
    }
}

Mime::~Mime()
{
}

std::map<std::vector<std::string>, std::string>	Mime::getMime()const{
	return mime;
}

std::string Mime::operator[](std::string const &key)
{
    std::map<std::vector<std::string>, std::string>::iterator it;
    for (it = mime.begin(); it != mime.end(); it++)
    {
        if (std::find(it->first.begin(), it->first.end(), key) != it->first.end())
            return (it->second);
    }
    return ("text/plain");
}
