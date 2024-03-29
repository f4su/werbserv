
#include	"../inc/launch_servers.hpp"
#include	"../inc/ServerException.hpp"
#include	<unistd.h>

using std::cout;

typedef sockaddr sa;
typedef sockaddr_in sa_in;

void	prepare_sockets(vector<Server> &servers){
	if (servers.size() < 1){
			throw ServerException("Not enough servers");
	}
	vector<Server>::iterator	it;

	for (it = servers.begin(); it != servers.end(); ++it){
		it->setSocket(socket(AF_INET, SOCK_STREAM, 0));
		if (it->getSocket() == -1){
			close_sockets(servers);
			throw ServerException("Error when creating the socket");
		}
		sa_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(it->getPort());
		addr.sin_addr.s_addr = htonl(it->getHost());
		if (bind(it->getSocket(), (const sa*)&addr, sizeof(addr)) == -1){
			close_sockets(servers);
			throw ServerException("Error when binding port. Review port permissions and host");
		}
		if (listen(it->getSocket(), 20) == -1){
			close_sockets(servers);
			throw ServerException("Error when listening through port");
		}
		std::cout << "\n+++ Socket " << it->getSocket() << " binded to port " << it->getPort() << " and listening to connections +++" << std::endl;
	}
}

std::string displayHiddenChars(string& str) {
    std::string result;
		char c;
    for (string::iterator it = str.begin(); it != str.end(); ++it) {
				c = *it;
        switch (c) {
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\r':
                result += "\\r";
                break;
            default:
                if (c < ' ' || c > '~') {
                    result += "\\x";
                    result += "0123456789abcdef"[((unsigned char)c) >> 4];
                    result += "0123456789abcdef"[((unsigned char)c) & 0xf];
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

void	calculate_max(vector<Server> &servers){
	int	max = 0;

	if (servers.empty()){
		return ;
	}
	for (vector<Server>::iterator	it = servers.begin(); it != servers.end(); ++it){
		if (it->getSocket() > max){
			max = it->getSocket();
		}
		if (!it->getClients().empty()){
			vector<int> clients = it->getClients();
			for (vector<int>::iterator jt = clients.begin(); jt != clients.end(); ++jt){
				if (*jt > max){
					max = *jt;
				}
			}
		}
	}
	servers[0].setMax(max);
}

void	close_sockets(vector<Server> &servers){
	vector<int>	clients;

	for (vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it){
		if (close(it->getSocket()) != 0){
			std::cerr << RED << "Error when closing fd " << it->getSocket() << EOC << std::endl;
		}
		if (!it->getClients().empty()){
			clients = it->getClients();
			for (std::vector<int>::iterator jt = clients.begin(); jt != clients.end(); ++jt){
				if (close(*jt) != 0){
					std::cerr << RED << "Error when closing fd " << *jt << EOC << std::endl;
				}
			}
		}
	}
}
