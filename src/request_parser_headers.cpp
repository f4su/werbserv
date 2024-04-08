#include	"../inc/request_parser.hpp"
#include	"../inc/headers.hpp"
#include	"../inc/Utils.hpp"

using	std::cout;

int count_chars(const string& str, char targetChar) {
    int count = 0;
    for (string::const_iterator ch = str.begin(); ch != str.end(); ++ch) {
        if (*ch == targetChar) {
            count++;
        }
    }
    return count;
}

vector<string> unite_tokens_between_commas(vector<string> &tokens){
	if (tokens.size() < 3){
		return tokens;
	}
	size_t	start = 0;
	size_t	end = 0;
	size_t	index = 0;
	bool		firstTime = true;
	int			count;
	string	result;
	vector<string>	line;
	vector<string>::iterator	jt = tokens.begin(); 

	for (vector<string>::iterator	it = tokens.begin(); it != tokens.end(); ++it){
		count = count_chars(*it, '"');
		//cout << CYA << "It = [" << *it << "] index->" << index << " and count ->" << count << EOC << std::endl;
		if (count > 0 && count % 2 != 0 && index > 0){
			if (firstTime){
				start = index;
				firstTime = false;
			}
			else {
				end = index;
				firstTime = true;
			}
			//cout << CYA << "Start = [" << start << "] end->" << end << EOC << std::endl;
			if (start > 0 && end > 0 && end > start){
				for (size_t i = start; i <= end && end <= tokens.size(); ++i){
					result += jt[i];
					if (i < end){
						result += " ";
					}
				}
				//cout << CYA << "****-> Result is ->" << result << EOC << std::endl;
				line.push_back(result);	
				result.clear();
				start = 0;
				end = 0;
			}
		}
		else {
			line.push_back(*it);	
		}
		++index;
	}
	return (line);
}

vector<string> merge_strings(vector<string> &tokens){
	size_t	size = 0;
	string	result;
	vector<string>	value;

	for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it, ++size){
		if (it == tokens.begin()){
			value.push_back(*it);
		}
		result += *it;
		if (size < tokens.size() -1){
			result += " ";
		}
	}
	value.push_back(result);
	return (value);
}

void	insert_values(string const &word, vector<string> &values){
	vector<string>	splitted;
	string	coma(",");

	splitted = ft_split(word, coma);
	for (vector<string>::iterator it = splitted.begin(); it != splitted.end(); ++it){
		values.push_back(*it);
	}
}

bool	check_headers_values(URI &rq, Server &server){
	string					valid_hdrs(IMPLEMENTED_HEADERS), key;
	mapStrVect			hdrs = rq.getHeaders();
	bool						hostHdr = false;
	vector<string>	values, hosts = server.getServerNames();
	size_t					port = 80;

	for (mapStrVect::iterator	it = hdrs.begin(); it != hdrs.end(); ++it){
		values = hdrs[it->first];
		key = it->first;
		if (valid_hdrs.find(it->first) == string::npos) continue ;
		cout << MAG << "key~~~~~~~~~~" << key << EOC << std::endl;
		if (key == CONTENT_TYPE_H){
			cout << MAG << "val~~~~~~~~~~" <<  hdrs[CONTENT_TYPE_H][0]  << EOC << std::endl;
		}

		if (key == TRANSFER_ENCODING_H){
				if (find(values.begin(), values.end(), "chunked") != values.end()){
					rq.setIsChunked(true);
				}
		}
		else if (key == EXPECT_H){
				if (values.size() == 1 && find(values.begin(), values.end(), "100-continue") != values.end()){
					rq.setExpectContinue(true);
				}
				else {
					cout << RED << "Error: Expect value not implemented (417 Expectation Failed)" << EOC << std::endl; 
					rq.setStatusCode(STATUS_417);
					return (true);
				}
			}
		else if (key == HOST_H){
			if (values.size() == 1){
				size_t pos = values[0].find(":");
				if (std::count(values[0].begin(), values[0].end(), ':') > 1 || pos == 0 || values[0].back() == ':'){
					cout << RED << "Error: Invalid Host (400)" << EOC << std::endl; 
					rq.setStatusCode(STATUS_400);
					return (true);
				}
				if (pos != string::npos){
					rq.setHost(values[0].substr(0, pos));
					std::stringstream ss(values[0].substr(values[0].find(":") + 1, values[0].size()));
					ss >> port;
					if (ss.fail()){
						cout << RED << "Error: Couldn't convert host port header string to nb (500)" << EOC << std::endl; 
						rq.setStatusCode(STATUS_500);
						return (true);
					}
					rq.setPort(port);
					cout << RED << "Port: [" << rq.getPort() << "]" << EOC << std::endl; 
				}
				else {
					rq.setHost(values[0]);
				}
			}



			cout << "HOOOOSTIEEEE->" << rq.getHost() << "]" << std::endl;
			if (values.size() != 1 || ( rq.getHost() != "127.0.0.1" &&
						find(hosts.begin(), hosts.end(), rq.getHost()) == hosts.end())){
				cout << RED << "Error: Host header doesn't match Server Names (400)" << EOC << std::endl; 
				rq.setStatusCode(STATUS_400);
				return (true);
			}
			hostHdr = true;
		}

		else if (key == CONTENT_TYPE_H && hdrs[CONTENT_TYPE_H][0] == "multipart/form-data"){
			cout << CYA << "Setting Boooooooooooooooooooooooooooundary" << EOC << std::endl;
			if ( hdrs[CONTENT_TYPE_H].size() == 1 || hdrs[CONTENT_TYPE_H][1].find("=") ==  string::npos){
				rq.setStatusCode(STATUS_400);
				return (true);
			}
			cout << CYA << "Setting Boooooooooooooooooooooooooooundary2222" << EOC << std::endl;
			rq.setBoundary(hdrs[CONTENT_TYPE_H][1].substr( hdrs[CONTENT_TYPE_H][1].find("=") + 1));
			rq.setIsMultipart(true);
		}

	}
	if (hostHdr == false){
		rq.setStatusCode(STATUS_400);
		cout << RED << "Error: Host header not detected" << EOC << std::endl; 
		return (true);
	}
	return (false);
}

//rfc7230 says that no whitespace is accepted between field-name and :
//Therefore, this is the syntax:			field-name: OWS field-content OWS
//OWS = Optional WhiteSpace						/r/n
bool invalid_header(vector<vector<string> > &tokens, URI &rq, Server &server)
{
	size_t															pos = 0, index = 0;
	mapStrVect													hdrs;
	vector<string>											values, line;
	vector<vector<string> >::iterator		it;
	string															key;

	cout << RED << "Starting Headers Parse" << EOC << std::endl; 
	for (it = tokens.begin(); it != tokens.end() && index++ <= rq.getHeadersSize(); ++it){
		if (it == tokens.begin() || it->size() < 1){
			continue ;
		}

		if ((pos = (*it)[0].find_first_of(':')) == string::npos){
			rq.setStatusCode(STATUS_400);
			return (true);
		}
		key = (*it)[0].substr(0, pos);
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);
		if (key == "USER-AGENT"){
			line = merge_strings(*it);
		}
		else {
			line = unite_tokens_between_commas(*it);
		}
		switch (line.size()){
			case 1:
				if (line[0].size() < 3 || line[0].front() == ':' || line[0].back() == ':' || pos != line[0].find_last_of(':')){
					cout << RED << "Error on a one-token header" << EOC << std::endl; 
					rq.setStatusCode(STATUS_400);
					return (true);
				}
				insert_values(line[0].substr(pos+1, line[0].length()), values);
				break ;

			case 2:
				if (line[0].size() < 2 || line[0].front() == ':' || line[0].back() != ':' || pos != line[0].find_last_of(':')){
					cout << RED << "Error on a two-tokens header" << EOC << std::endl; 
					rq.setStatusCode(STATUS_400);
					return (true);
				}
				insert_values(line[1], values);
				break ;

			default :
				if (line[0].size() < 2 || line[0].front() == ':' || line[0].back() != ':' || pos != line[0].find_last_of(':')){
					cout << RED << "Error on a multiple tokens header" << EOC << std::endl; 
					rq.setStatusCode(STATUS_400);
					return (true);
				}
				for (size_t word = 1; word < line.size(); ++word){
					cout << CYA << "[" << line[word] << "]" << EOC << std::endl; 
					if (((word == line.size() -1) && (line[word].back() == ',')) ||
							((word < line.size() - 1) && ((line[word].back() != ',' && line[word].back() != ';') || line[word].size() <= 1))){
						cout << RED << "Error on case 4" << EOC << std::endl; 
						rq.setStatusCode(STATUS_400);
						return (true);
					}
					if (word < line.size() - 1){
						insert_values(line[word].substr(0, line[word].size() - 1), values);
					}
					else{
						insert_values(line[word], values);
					}
				}
				break ;
			}
			hdrs[key] = values;
			values.clear();
			key.clear();
	}


	/*cout << CYA << "\n\n\n*********HEADERS*************" << EOC << std::endl << std::endl;
	for (mapStrVect::iterator	it = hdrs.begin(); it != hdrs.end(); ++it){
	
		cout << CYA << "Key->[" << it->first << "]" << EOC << std::endl << std::endl;

		cout << CYA << "Vals->[" ;
		for (vector<string>::iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
			cout << *jt << " , ";
		}
		cout << "]" << EOC << std::endl << std::endl;
	}	
	cout << CYA << "\n\n\n*********HEADERS END*************" << EOC << std::endl;*/

	if (hdrs.size() == 0){
		cout << RED << "Error when storing the headers" << EOC << std::endl; 
		rq.setStatusCode(STATUS_500);
		return (true);
	}
	rq.setHeaders(hdrs);
	if (check_headers_values(rq, server)){
		cout << RED << "Error on headers values" << EOC << std::endl; 
		return (true);
	}
	//transfer_encoding(rq);
	return (false);
}
