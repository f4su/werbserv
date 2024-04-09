#include "../inc/Response.hpp"
#include "../inc/headers.hpp"
#include "../inc/launch_servers.hpp"

#define	CRLF			"\r\n"
#define	CRLFx2				"\r\n\r\n"

Response::Response(URI &rq){
    mime = Mime();
    isListing = false;
    request = &rq;
    code = request->getStatusCode();
		uploadDir = "";	
}

Response::~Response(){
}





void Response::handleResponse(Server &server, Route &route){
    try
    {
				if (route.getAllowListingSet() == true)
					route.setAllowListing(route.getAllowListing());
				else
					route.setAllowListing(server.getAllowListing());
				headers["Content-Type"] = "text/html";
				errorPages = route.getErrorPages().size() ? route.getErrorPages() : server.getErrorPages();
				uploadDir = route.getUploadDir();
        if (request->getMethod() == 'g'){
            std::cout << RED << "IN GET "<< EOC << std::endl;
            handleGet(server, route);
        }
        else if (request->getMethod() == 'p'){
            std::cout << RED << "IN POST "<< EOC << std::endl;
            handlePost(server, route);
        }
        else if (request->getMethod() == 'd'){ 
            std::cout << RED << "IN DELETE"<< EOC << std::endl;
            handleDelete(server);
        }
        else {
            std::cout << RED << "IN EXCEPT"<< EOC << std::endl;
            throw ServerException(STATUS_501);			
		}
    }
    catch (ServerException & e)
    {
        std::cout << CYA << "MSGGGGGGGGGGGG>" << e.what() << EOC << std::endl;
        int c;
        std::string strnb = e.what();
        std::stringstream    ss(strnb.substr(0, 3));
        ss >> c;
        request->setStatusCode(strnb);
				string	msg(e.what());
        std::cout << CYA << "C is " << c << " for msg ->" << msg << EOC << std::endl;
				if (errorPages.size() && errorPages.find(c) != errorPages.end()){
        	std::cout << CYA << "Enteeeerrrrrrrrrrrrrrrrrrrris " << c << EOC << std::endl;
        	readContent(errorPages[c], e.what());
				}
				else {
    			body = "<!DOCTYPE html><html><h1 align='center'>" + msg + "</h1></html>";
				}
    }
}
















void Response::handleGet(Server &server, Route const & route)
{
		server.getRoot(); //Compile

    std::cout << MAG << "IN GET" << EOC << std::endl;
		string filePath = request->getPath();
    checkRedirection(route);
    if (route.getAllowListing()){
        vector<string> files = getFilesInDirectory(filePath);
        body = generateHtmlListing(files);
        return ;
    }


    removeConsecutiveChars(filePath, '/');  //Borrar luego


    if (!route.getCgi().empty())
    {
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        if (Cgiheaders.find("Content-Type") == Cgiheaders.end())
            headers["Content-Type"] = "text/html";
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readContent(filePath, STATUS_200);
}

void Response::handlePost(Server &server, Route const & route)
{
    std::cout << MAG << "IN POST" << EOC << std::endl;
		server.getRoot(); //compile
    if (!route.getCgi().empty())
    {
        string filePath = request->getPath(); //getFilePath(server, route);
        removeConsecutiveChars(filePath, '/');
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readBody(route);
    throw ServerException(STATUS_201);
}

void Response::handleDelete(Server &server)
{
    std::cout << MAG << "IN DELETE" << EOC << std::endl;
		server.getRoot(); //for compile
    string filePath = request->getPath(); //getFilePath(server, route);
    removeFileOrDirectory(filePath);
    throw ServerException(STATUS_204);
}











size_t	setStatusNb(string &code){
	if (code.size() < 4)
		return 0;

	string						strNb = code.substr(0, 3);
	int 							result		= 0;
	std::stringstream	ss(strNb);
	ss >> result;
	cout << "RRRRRRRRResult -> " << result << std::endl;
	if (ss.fail() || result < 400 || result > 599){
		return 0;
	}
	return result;
}

string	add_error_page_response(string &filePath, string &code){
	string				response;
	std::ifstream file(filePath);

	if (file.is_open())
	{
		std::stringstream buffer;
		string line;
		string body;
		while (std::getline(file, line))
				buffer << line << std::endl;
		body = buffer.str();
		file.close();

		response += body;
		return response;
	}
	else{
    return ("<!DOCTYPE html><html><h1 align='center'>" + code + "</h1></html>");
	}
}

void Response::readContent(string const &filePath, string code)
{
		size_t	statusNb = 0;

    std::cout << CYA << "Enters Read Content" << filePath << " <---readContent(string const &filePath, string code)" << EOC << std::endl;
		if (filePath.size() && filePath.at(filePath.size() -1) == '/'){
    	code = code == STATUS_200 ? STATUS_404 : code;
			statusNb = setStatusNb(code);
    	std::cout << CYA << "Status Nb is " << statusNb << EOC << std::endl;
			if (errorPages.size() && errorPages.find(statusNb) != errorPages.end()){
    		std::cout << CYA << "Going to set ERR PAGEE!!" << EOC << std::endl;
				body = add_error_page_response(errorPages[statusNb], code);
			}
			else{
    		body = "<!DOCTYPE html><html><h1 align='center'>" + code + "</h1></html>";
			}
			return ;
		}
    std::ifstream file(filePath);
    //std::cout << CYA << "FILEPATH IS : " << filePath << " <---readContent(string const &filePath, string code)" << EOC << std::endl;

	if (file.is_open())
    {
        headers["Content-Type"] = mime[Cgi::getFileExt(filePath)];
    		//std::cout << CYA << "Content TYpe is->" << headers["Content-Type"] << EOC << std::endl;
				std::stringstream buffer;
        string line;
        while (std::getline(file, line))
            buffer << line << std::endl;
        body = buffer.str();
        file.close();
    }
    else
    {
        code = code == STATUS_200 ? STATUS_404 : code;
    		std::cout << CYA << "Code before SetStatus" << code << EOC << std::endl;
				statusNb = setStatusNb(code);

    		std::cout << CYA << "Second Status Nb is " << statusNb << EOC << std::endl;
				if (errorPages.size() && errorPages.find(statusNb) != errorPages.end()){
    			std::cout << CYA << "Going to set ERR PAGEE!!" << EOC << std::endl;
					body = add_error_page_response(errorPages[statusNb], code);
				}
				else{
					body = "<!DOCTYPE html><html><h1 align='center'>" + code + "</h1></html>";
				}
    }
}


string Response::tryFiles(Server const & server, Route const & route, string & root)
{
    if (route.getRouteType() == Route::DIRECTORY)
        root = route.getRoot();
    vector<string> indexes;
    if (route.getRouteType() == Route::FILE)
        indexes.push_back(route.getPath());
    else
        indexes = route.getIndex().empty() \
            ? (server.getIndex().empty() ? vector<string>() : server.getIndex()) \
            : route.getIndex();
    indexes.push_back("index.html");
    string filePath;
    for (vector<string>::iterator it = indexes.begin(); it != indexes.end(); it++)
    {
        filePath = root + "/" + *it;
        std::ifstream file(filePath);
        if (file.good())
        {
            file.close();
            return *it;
        }
        file.close();
    }
    //std::cout << CYA << "FILEPATH IS : " << filePath << " <---tryFiles(Server const & server, Route const & route, string & root)" << EOC << std::endl;
    if (route.getRouteType() != Route::FILE)
    {
        throw ServerException(STATUS_403);
    }
    throw ServerException(STATUS_404);
}


string	Response::resolveFileName(){
	string	filename;
	if (request->getParams().size() && request->getParams().find("filename") != request->getParams().end()){
  	filename = (uploadDir + "/" + request->getParams()["filename"]);
	}
	else {
  	filename = (uploadDir + "/" + "defaultUploadFileName-" + getDateGMT());
	}
	if (filename.size() && filename[0] == '/'){
		filename = "." + filename;
	}
	return filename;
}

void Response::processUrlEncodedBody(const string& body)
{
    std::map<string, string> queryStrings;
    vector<string> params = ft_split(body, "&");

    std::cout << RED << "DOING URL ENCODED--- " << EOC << std::endl;
    for (vector<string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        vector<string> param = ft_split(*it, "=");
        if (param.size() == 2)
            queryStrings[param[0]] = param[1];
    }

		std::istringstream ss(body);
		processFileUpload(ss, resolveFileName());

}

void Response::processFileUpload(std::istringstream& ss, const string& name)
{
		std::cout << MAG << "UPLOAAAAAAAAAD {{{{{{{" << name << EOC << std::endl;
    std::ofstream file(name.c_str());
    if (!file.is_open()) {
			std::cerr << RED << "Error uploading file!" << EOC << std::endl;
        throw ServerException(STATUS_500);
    }
    string content;
    //std::getline(ss, content);
    //std::getline(ss, content);
    while (std::getline(ss, content))
        file << content << "\n";

		//Crete a file
		file.close();
}

void Response::processFormField(std::istringstream& ss, const string& name, std::map<string, string>& queryStrings)
{
		std::cout << MAG << "Procesing FFFFFFFFFFFFFFFForm field" << EOC << std::endl;
    string content;
    //std::getline(ss, content);
    //std::getline(ss, content);
    std::getline(ss, content, '\0');
    queryStrings[name] = content;
}

string getFileName(string const &body, Route const &route, size_t length, string param, bool test)
{
		string name;
    int len = body.find("\"", body.find(param) + length) - body.find(param) - length;
		if (test == true)
    	name = route.getUploadDir() + "/" + body.substr(body.find(param) + length, len);
		else
			name = body.substr(body.find(param) + length, len);
		if (body.find(CRLF) == string::npos || name.size() == 0){
			throw ServerException(STATUS_400);
		}
		return name;
}

void Response::processMultipartFormDataBody(const string& body, Route const & route)
{
    std::map<string, string> queryStrings;
    string boundary = request->getBoundary();
    //vector<string> params = ft_split(body, "--" + boundary);

    std::cout << RED << "doing multipart--- with boundary[" << boundary << "]" << EOC << std::endl;

		string bod = body;
		std::cout << RED << "--- Body in multipart isssss[" << displayHiddenChars(bod) << "]" << EOC << std::endl;
		if (body.find(CRLFx2) == string::npos){
			std::cout << RED << "Error: multipart CRLFx2 not found" << EOC << std::endl;
			throw ServerException(STATUS_400);
		}
		string	contentBody = body.substr(body.find(CRLFx2) + 4);
		std::cout << RED << "--- ContentBody in multipart isssss[" << displayHiddenChars(contentBody) << "]" << EOC << std::endl;
		std::istringstream ss(contentBody);
		if (body.find("filename") != string::npos){
				processFileUpload(ss, getFileName(body, route, 10, "filename", true));
		}
		else if (body.find("name") != string::npos){
				processFormField(ss, getFileName(body, route, 6, "name", false), queryStrings);
		}

}



void Response::readBody(Route const & route)
{	
		if (request->getHeaders().find(CONTENT_TYPE_H) == (request->getHeaders().end())){
    	std::cout << RED << "Error: Content-Type header not found when reading body" << EOC << std::endl;
    	throw ServerException(STATUS_400);
		}

		mapStrVect		headers = request->getHeaders();
		const vecStr	&contentType = headers[CONTENT_TYPE_H];

    const string& body = request->getBody();
    
    //std::cout << RED << "Content Type is->[" << contentType[0] << "]" << EOC << std::endl;
    if (contentType.size() && contentType[0] == "application/x-www-form-urlencoded")
        processUrlEncodedBody(body);		//Mirar cuándo ocurre estoo
    else if (contentType.size() && contentType[0] == "multipart/form-data")
        processMultipartFormDataBody(body, route);
		else
		{
				std::istringstream ss(body);
				processFileUpload(ss, resolveFileName());
		}
}



string getDateGMT()
{
    std::time_t t = std::time(0);
    std::tm tm = *std::gmtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
    return (ss.str());
}

string Response::getResponse()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << request->getStatusCode() << CRLF;
    ss << "Server: webserv/1.0" << CRLF;
    ss << "Date: " << getDateGMT() << CRLF;
    ss << "Content-Length: " << toString(body.size()) << CRLF;
    for (std::map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
        ss << it->first << ": " << it->second << CRLF;
    ss << CRLF;
    ss << body;

    return (ss.str());
}
