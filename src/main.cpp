#include "../inc/request_parser.hpp"
#include "../inc/launch_servers.hpp"
#include "../inc/Server.hpp"
#include "../inc/ServerException.hpp"
#include "../inc/Config.hpp"


void	resolveAllowListing(vector<Server> &servers){

	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it){
		vector<Route>	routes = it->getRoutes();
		for (vector<Route>::iterator jt = routes.begin(); jt != routes.end(); ++jt){
			cout << MAG << "ALLOW LISTING IS-->" << jt->getAllowListing() << "] on path >" << jt->getPath() << EOC << std::endl;
			//AllowListing
			if (jt->getAllowListingSet() == false){
				cout << MAG << "SETTING PAPA-->" << it->getAllowListing() << "] on path >" << it->getRoot() << EOC << std::endl;
				jt->setAllowListing(it->getAllowListing());
			}
		}
	}
}


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
		resolveAllowListing(servers);

		cout << MAG << "KAPOOOOOUETEEE-->" << servers[0].getRoutes()[1].getAllowListing() << "] on path >" << servers[0].getRoutes()[1].getPath() << EOC << std::endl;

		prepare_sockets(servers);
		listening_connections(servers);
	}
	catch (ServerException &e) {
		cerr << RED << "\nError: " << e.what() << EOC << std::endl;
	}
	return (0);
}
