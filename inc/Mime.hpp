#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

class Mime
{
    private:
        std::map<std::vector<std::string>, std::string>	mime;

    public:
        Mime();
        ~Mime();
        void print();
        std::map<std::vector<std::string>, std::string>	getMime()const;
        std::string operator[](std::string const &key);
};
