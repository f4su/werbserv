#include "../inc/request_parser.hpp"
#include "../inc/launch_servers.hpp"
#include "../inc/Server.hpp"
#include "../inc/ServerException.hpp"
#include "../inc/Config.hpp"

int main(int argc, char **argv) 
{
	//DEBUG REQUEST PARSER TESTING - Remove before delivery!!!!
	/*
	if (argc == 2 && argv[1][0] == 'z'){
		request_testing();
		return (0);
	}*/

	std::string cnfPth;
	argc == 1 ? cnfPth = "./test" : argc == 2 ? cnfPth = argv[1] : cnfPth = nullptr;
	try {
		Config::parse(cnfPth);
		vector<Server> servers = Config::getServers();
		prepare_sockets(servers);
		listening_connections(servers);
	}
	catch (ServerException &e) {
		cerr << RED << "\nError: " << e.what() << EOC << std::endl;
  }

	return (0);
}
