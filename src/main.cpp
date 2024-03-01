#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/errno.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <fstream>
#include "../inc/idk.hpp"

#include	"../inc/request_parser.hpp"

int main(int argc, char **argv) 
{
	std::string cnfPth;
	argc == 1 ? cnfPth = "./test" : argc == 2 ? cnfPth = argv[1] : cnfPth = nullptr;
	try {
		Config::parse(cnfPth);
		std::vector<Server> servers = Config::getServers();
	}
	catch (ServerException &e) {
		std::cout << "Nop\n";
  }


	//Testing the request parser
	std::vector<std::string> requests;

	//	CHECKING FIRST LINE
		//---lines---
			//0. empty
			requests.push_back("");
			//1. 1 line
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\n");
			//2. 2 lines
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n");
			//3. 3 lines without \r
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\nJuan: Paco\r\n\r\n");

		//---tokens---
			//4. 3 lines ✅
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//5. 3 lines - bad tokens in first line
			requests.push_back("GET http: //localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---tokens---
			//6. 3 lines - bad method
			requests.push_back("GETT http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//7. 3 lines - bad method
			requests.push_back("delete http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//8. 3 lines - bad method
			requests.push_back("PoST http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//9. 3 lines - bad method
			requests.push_back("PUT http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---absolute form scheme---
			//10. 3 lines - bad scheme
			requests.push_back("GET https://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//11. 3 lines - bad scheme
			requests.push_back("GET localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---absolute form host---
			//12. 3 lines - empty host
			requests.push_back("GET http://:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//13. 3 lines - bad host (IPv4)	-  don't need to handle since it will reponse "Host not found"✅
			requests.push_back("GET http://1.1.1:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
	
				//14. 3 lines - bad host (IPv6)	- don't need to handle since it will reponse "Host not found"✅

				//15. 3 lines - bad host (IPv6) - is bad since closing [ ] is a syntax error that we should parse

		//---authority form host---
			//14. 3 lines - wrong form
			requests.push_back("GET localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//15. 3 lines - wrong form
			requests.push_back("GET localhost:80/index?key1=value1&key2=value2 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//16. 3 lines - wrong form
			requests.push_back("GET localhost:80/index HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//17. 3 lines - wrong form
			requests.push_back("GET localhost:80/ HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//18. 3 lines - wrong form
			requests.push_back("GET localhost:80#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//19. 3 lines - empty host
			requests.push_back("GET :80 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//20. 3 lines - bad host - we don't need to handle IPv4 correction✅
			requests.push_back("GET 1.1.1:80 HTTP/1.1\r\nJuan: Paco\r\n\r\n");

				//14. 3 lines - bad host (IPv6)	- (correct), don't need to handle since it will reponse "Host not found"✅

				//15. 3 lines - bad host (IPv6) - is bad since closing [ ] is a syntax error that we should parse
	

		//---authority form port---
			//21. 3 lines - emty port (it takes default port 80)✅
			requests.push_back("GET localhost: HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//22. 3 lines - no port (it takes default port 80) ✅
			requests.push_back("GET localhost HTTP/1.1\r\nJuan: Paco\r\n\r\n");
	
				//15. 3 lines - Wrong port (alpha)
				//15. 3 lines - Wrong port (too long)


		//---absolute form port---
			//23. 3 lines - emty port ✅
			requests.push_back("GET http://localhost:/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//24. 3 lines - no port (it takes default port 80) ✅
			requests.push_back("GET http://localhost/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//25. 3 lines - bad port 
			requests.push_back("GET http://localhost:a/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
				//15. 3 lines - Wrong port (alpha)
				//15. 3 lines - Wrong port (too long)

		//---absolute form path---
			//26. 3 lines - no path
			requests.push_back("GET http://localhost:80?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//27. 3 lines - "bad" path (we don't parse the path, we just try to open it) ✅
			requests.push_back("GET http://localhost:80//index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---authority form path---
			//28. 3 lines - no path ✅
			requests.push_back("GET localhost:80 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//29. 3 lines - bad path
			requests.push_back("GET localhost:80//index HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//30. 3 lines - path
			requests.push_back("GET localhost:80/index HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---origin form path---
			//31. 3 lines - no path
			requests.push_back("GET a/ HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//32. 3 lines - bad path ✅
			requests.push_back("GET // HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//33. 3 lines - ✅
			requests.push_back("GET / HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---absolute form query---
			//34. 3 lines - bad query
			requests.push_back("GET http://localhost:80/index?=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//35. 3 lines - bad query
			requests.push_back("GET http://localhost:80/index?key1=&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//36. 3 lines - bad query
			requests.push_back("GET http://localhost:80/index?=&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//37. 3 lines - bad query
			requests.push_back("GET http://localhost:80/index?key1=value1&&key2=value2#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//38. 3 lines - no query, but section ✅
			requests.push_back("GET http://localhost:80/index#section1 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//39. 3 lines - query, but no section ✅
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2 HTTP/1.1\r\nJuan: Paco\r\n\r\n");
			//40. 3 lines - no query, section ✅
			requests.push_back("GET http://localhost:80/index HTTP/1.1\r\nJuan: Paco\r\n\r\n");

		//---absolute form Protocol---
			//41. 3 lines - bad Protocol
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1.\r\nJuan: Paco\r\n\r\n");
			//42. 3 lines - bad Protocol
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 http/1.1\r\nJuan: Paco\r\n\r\n");
			//43. 3 lines - bad Protocol
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.\r\nJuan: Paco\r\n\r\n");

	//	CHECKING HEADERS
		//---lines---
			//44. 3 lines - several headers without \r
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1 \r\nJuan:Paco\nPedro:  Pablo\n\r\nCoooonteent");

		//---tokens---
			//45. 3 lines - bad tokens in second line
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan: Paco .\r\n\r\n");
			//46. 3 lines - bad tokens in second line
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\n\r\n\r\n");
	
		//---key-values---
			//47. 3 lines - :value in one token header
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\n:Paco\r\n\r\n");
			//48. 3 lines - :value in two token header
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\n: Paco\r\n\r\n");
			//49. 3 lines - key: in one token header
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan:\r\n\r\n");
			//50. 3 lines - key: in one token header
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\nJuan :\r\n\r\n");
			//51. 3 lines - : in header
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1\r\n:\r\n\r\n");
			//52. 3 lines - several headers ✅
			requests.push_back("GET http://localhost:80/index?key1=value1&key2=value2#section1 HTTP/1.1 \r\nJuan:Paco\r\nPedro:  Pablo\r\n\r\nCoooonteent");


	std::vector<std::string>::const_iterator	it;
	std::cout << "\nTest size is: " << requests.size() << "\n";
	for (it = requests.cbegin(); it != requests.cend(); ++it){
		std::string	state;
		int	index = std::distance(requests.cbegin(), it);
		invalid_request(it->c_str()) ? state = "\033[1;31minvalid\033[0m" : state = "\033[1;32mcorrect\033[0m";
		std::cout << "\033[1;37mRequest nº" << index << " is " << state << "\033[0m\n";
		if (index == 4 || index == 13 || (index >= 20 && index <= 24) || index == 27 || index == 28 || index == 32 || index == 33 || (index > 37 && index < 41) || index == 52){
			if (state == "\033[1;31minvalid\033[0m"){
				std::cout <<  "\033[1;31m---TEST FAILED---\033[0m";
				return (1);
			}
		}
		else {
			if (state == "\033[1;32mcorrect\033[0m"){
				std::cout <<  "\033[1;31m---TEST FAILED---\033[0m";
				return (1);
			}
		}
	}
	return (0);
}

/*std::string FileToString(std::istream& file)
{
    std::string str;
    char c;
    while(file.get(c)) str += c ;
    return (str);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "error arguments\n";
		return (1);
	}
	int	serv_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (serv_socket == -1)
	{
		std::cout << "error in socket\n";
		return (1);
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[1]));
	address.sin_addr.s_addr = INADDR_ANY;
	if (bind(serv_socket, (const sockaddr *)&address, sizeof(address)) == -1)
	{
		std::cout << "error in bind\n";
		return (1);
	}
	if (listen(serv_socket, 20) == -1)
	{
		std::cout << "error in listen\n";
		return (1);
	}
	fd_set readfds;
	int	fdMax = serv_socket;
	FD_ZERO(&readfds);
	FD_SET(fdMax, &readfds);
	fd_set	fdNow;
	while (1)
	{
		fdNow = readfds;
		if (select(fdMax + 1, &fdNow, NULL, NULL, NULL) == -1)
		{
			std::cout << "select in accept\n";
			continue ;
		}
		std::cout << "PASSED SELECT"<< std::endl;
		for (int i = 0; i <= fdMax; i++)
		{
			std::cout << "--------------------------------\n" << "I: " << i << std::endl << "SERV_SOCKET: " << serv_socket << std::endl << std::endl;
			if (!FD_ISSET(i, &fdNow))
				continue ;
			std::cout << "PASSED ISSET\n";
			if (i == serv_socket)
			{
				socklen_t address_len = sizeof(address);
				int client = accept(serv_socket, (sockaddr *)&address, &address_len);
				std::cout << "CLIENT: " << client << std::endl;
				if (client == -1)
				{
					std::cout << "error in accept\n";
					continue ;
				}
				fdMax = fdMax > client ? fdMax : client;
				std::cout << "FDMAX: " << fdMax << std::endl;
				FD_SET(client, &readfds);
				break;
			}
			char r_buff[1024];
			int	readed = recv(i, r_buff, 1024, 0);
			std::cout << "READED: " << readed << std::endl;
			if (readed == -1)
			{
				std::cout << "error in recv\n";
				continue ;
			}
		//	std::ifstream file("html/index.html");
		//	std::string s_buff = FileToString(file);
		//	std::string test = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: ";
		//	std::string size = std::to_string(sizeof(s_buff));
		//	std::string test2 = "\r\n\n";
		//	s_buff = test + size + test2 + s_buff; 
		//	std::cout << "-------------------\n" << s_buff << "\n---------------------\n" ;
		//	std::cout << "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: 13\r\n\n<h1>Hola</h1>" << "\n";
			std::string s_buff = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: 13\r\n\n<h1>Hola</h1>";
			if (send(i, s_buff.c_str(), strlen(s_buff.c_str()), 0) == -1)
			{
				std::cout << "error in send\n";
				continue ;
			}
			std::cout << "connected to client\n";
			std::cout << "ERRNO: " << errno << std::endl << "FDMAX: " << fdMax << std::endl << "--------------------------------\n";
		}
	}
	return (0);
}*/
