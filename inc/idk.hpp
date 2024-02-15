#pragma once

# define DEFAULT_ROOT "/var/www/html"

#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <csignal>
#include <fcntl.h>
#include <vector>
#include <map>
#include <stack>
#include <sys/stat.h>
#include <dirent.h>

#include "Utils.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "Config.hpp"
#include "ServerException.hpp"