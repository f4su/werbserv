
#include	"../inc/request_parser.hpp"
#include	"../inc/Utils.hpp"
#include	<sstream>

using	std::cout;

bool	invalid_values(const string &token, URI &rq_uri, size_t p_start, size_t pr_start, size_t f_start, char *form){

	//string	formStr(form);
	cout << "Form iss -> [" << form[0] << "]" << std::endl;
	//Authority (optinal) must begin with // and end with /, ?, # or the end of the URI. It doesn't appear in origin form
	if ((*form == 'a' && p_start !=  string::npos) ||
			(*form == 'y')){
		//Shortest authority possible:		http://a/			a
		if (*form == 'a'){
			rq_uri.setAuthority(token.substr(7, p_start - 7));
		}
		else{
			rq_uri.setAuthority(token);
		}
		cout << "Authority iss -> " << rq_uri.getAuthority() << std::endl;
		//check auth syntax & port
		if (invalid_authority(rq_uri.getAuthority(), rq_uri)){
			cout << RED << std::endl << "\tRequest error: Invalid URI authority syntax" << EOC << std::endl;
			return (true);
		}
	}

	//Path must begin with a / and end with ?, # or the end of the URI. It's not applicable in Authority form
	if (p_start != string::npos){
		if (pr_start != string::npos){
			rq_uri.setPath(token.substr(p_start, pr_start - p_start));
		}
		else if (f_start != string::npos){
			rq_uri.setPath(token.substr(p_start, f_start - p_start));
		}
		else{
			rq_uri.setPath(token.substr(p_start, token.size() - p_start));
		}
	}
	else if (*form != 'y') {
		cout << RED << std::endl << "\tRequest error: URI without path" << EOC << std::endl;
		return (true);
	}

	//Params/Query (optional) must begin with a ? and end with # or the end of the URI. It's only applicable in absolute form
	if (pr_start != string::npos){
		f_start != string::npos ? rq_uri.setQuery(token.substr(pr_start, f_start - pr_start)) :
			rq_uri.setQuery(token.substr(pr_start, token.size() - pr_start));
		//check params syntax
		if (invalid_query_syntax(rq_uri.getQuery(), rq_uri)) {
			cout << RED << std::endl << "\tRequest error: Invalid URI params/query" << std::endl;
			return (true);
		}
	}

	//Fragment (optional) must begin with a # and end with the end of the URI. It's only applicable in absolute form
	if (f_start != string::npos){
		rq_uri.setFragment(token.substr(f_start, token.size() - f_start - 1));
	}
	return (false);
}

bool	invalid_authority(const string &auth, URI &rq_uri){
	//Authority is composed by:	host : port
	size_t	host_end = auth.find_last_of(':');
	size_t	ipV6in = auth.find("[");
	size_t	ipV6out = auth.find("]");
	
	cout << CYA << "Auth is [" << auth << "]" << EOC << std::endl;
	if (auth.size() < 1 || auth.front() == ':' ||
			(ipV6in != string::npos && auth.size() < 3) ||
			(ipV6in != string::npos && ipV6in != 0) ||
			(ipV6out != string::npos && ipV6out != auth.size() -1) ||
			(ipV6in != string::npos && ipV6out == string::npos) ||
			(ipV6in == string::npos && ipV6out != string::npos)) {
		return (true);
	}
	if (host_end == string::npos || auth.back() == ':'){
		host_end = auth.size();
		rq_uri.setPort(80);
	}
	else {
		if (invalid_port(auth.substr(host_end + 1, auth.size()), rq_uri)){
			cout << "Invalid port\n";
			return (true);
		}
	}
	ipV6in != string::npos ? rq_uri.setHost(auth.substr(1, --host_end)) :
		rq_uri.setHost(auth.substr(0, host_end));
	return (false);
}

bool	invalid_port(const string &port, URI &rq_uri){
	//Ports are store in 16bits. Therefore, 65,536 ports available	0 - 65535
	std::stringstream ss(port); 
	int num;

	cout << RED << "Port is [" << port << "]" << EOC << std::endl;
	if (port.find_first_not_of("0123456789") != string::npos){
		return (true);
	}
	if (ss >> num) { 
		if (num >= 65536){
			return (true);
		}
	}
	else {
			return (true);
	}
	rq_uri.setPort(num);
	return (false);
}

bool	invalid_query_syntax(const string &query, URI &rq_uri){
	//We will only accept & as separator for keys and values
	cout << "Query iss xxxx> " << query << "\n";
	if (query.size() <= 3 || query[0] != '?' || query.find("&&") != string::npos || query.find("==") != string::npos){
		return (true);
	}
	std::vector<std::string>	params;
	params = ft_split(query.substr(1, query.size()), "&");
	std::vector<std::string>::const_iterator it;
	std::map<string, string>	prms;
	for (it = params.begin(); it != params.end(); ++it){
		std::vector<std::string>	pairs = ft_split(*it, "=");
		//cout << "kkkkkkkkkk ---> " << *it << "\n";
		if (pairs.size() != 2){
			return (true);
		}
		//cout << "pair 0 )))> " << pairs[0] << "\npair 1 )))> " << pairs[1] << "\n";
		prms.insert(std::make_pair(pairs[0], pairs[1]));
	}
	rq_uri.setParams(prms);
	return (false);
}

