#include "../inc/Response.hpp"
#include "../inc/headers.hpp"

#define	CRLF			"\r\n"

Response::Response(URI &rq)
{
    mime = Mime();
    isListing = false;
    request = &rq;
    code = request->getStatusCode();
}

Response::~Response()
{
}





void Response::handleResponse(Server &server, Route &route)
{
    try
    {
        std::cout << RED << "ROUTE IN HANDLEREPONSE P--------> " << route.getPath() << EOC << std::endl;
		headers["Content-Type"] = "text/html";
        if (request->getMethod() == 'g'){
            std::cout << RED << "IN GET "<< EOC << std::endl;
            handleGet(server, route);
        }
        else if (request->getMethod() == 'p'){
            std::cout << RED << "IN POST "<< EOC << std::endl;
            handlePost(server, route);
        }
        else if (request->getMethod() == 'd'){ 
            std::cout << RED << "IN POST "<< EOC << std::endl;
            handleDelete(server, route);
        }
        else {
            std::cout << RED << "IN EXCEPT"<< EOC << std::endl;
            //setStatus(STATUS_501);
            throw ServerException(STATUS_501);			//TO DO: Todo lo que sean throws, convertirlos en send
		}
    }
    catch (ServerException & e)
    {
        std::cout << CYA << "MSGGGGGGGGGGGG>" << e.what() << EOC << std::endl;
       // std::cout << CYA << "///////////////////////CODE IS 1 : " << code << EOC << std::endl;
        //code = request->getStatusCode();
       // std::cout << CYA << "///////////////////////CODE IS 1 : " << code << EOC << std::endl;
        int c;
        std::string strnb = e.what();
        std::stringstream    ss(strnb.substr(0, 3));
        ss >> c;
        std::cout << CYA << "///////////////////////1st C IS 1: " << c << EOC << std::endl;
        //std::cout << CYA << "///////////////////////1st CODE IS 1: " << code << EOC << std::endl;
        std::cout << CYA << "///////////////////////1st FILEPATH IS 1: " << server.getErrorPages()[c] << EOC << std::endl;
        request->setStatusCode(strnb);
        readContent(server.getErrorPages()[c], e.what());
    }
}
















void Response::handleGet(Server &server, Route const & route)
{
    //string filePath = getFilePath(server, route);
    server.getAllowListing(); //compilar

    string filePath = request->getPath();
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---handleGet(Server &server, Route const & route)" << EOC << std::endl;
    checkRedirection(route);
    std::cout << CYA << "ALLOW LISTING--->" << route.getAllowListing() << EOC << std::endl;
    std::cout << CYA << "ROUTE PATH--->" << route.getPath() << EOC << std::endl;
    if (route.getAllowListing()){
        std::cout << CYA << "-----IS IN ALLOW LISTING---" << EOC << std::endl;
        vector<string> files = getFilesInDirectory(request->getPath());
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
    if (!route.getCgi().empty())
    {
        string filePath = getFilePath(server, route);
         std::cout << CYA << "FILEPATH IS : " << filePath << " <---handlePost(Server &server, Route const & route)" << EOC << std::endl;
        removeConsecutiveChars(filePath, '/');
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    readBody(route);
    //setStatus(STATUS_201);
    throw ServerException(STATUS_201);
}

void Response::handleDelete(Server &server, Route const & route)
{
    string filePath = getFilePath(server, route);
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---handleDelete(Server &server, Route const & route)" << EOC << std::endl;
    removeConsecutiveChars(filePath, '/');
    if (!route.getCgi().empty())
    {
        Cgi cgi(route, filePath, *request);
        std::map<string, string> Cgiheaders = cgi.getResponseHeaders();
        headers.insert(Cgiheaders.begin(), Cgiheaders.end());
        body = cgi.getResponseBody();
        return; 
    }
    removeFileOrDirectory(filePath);
}























void Response::readContent(string const &filePath, string code)
{
    std::ifstream file(filePath);
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---readContent(string const &filePath, string code)" << EOC << std::endl;

    if (file.is_open())
    {
        headers["Content-Type"] = mime[Cgi::getFileExt(filePath)];
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
        body = "<!DOCTYPE html><html><h1 align='center'>" + code + "</h1></html>";
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
    std::cout << CYA << "FILEPATH IS : " << filePath << " <---tryFiles(Server const & server, Route const & route, string & root)" << EOC << std::endl;
    if (route.getRouteType() != Route::FILE)
    {
        //setStatus(STATUS_403);
        throw ServerException(STATUS_403);
    }
    //setStatus(STATUS_404);
    throw ServerException(STATUS_404);
}

string Response::getFilePath(Server const & server, Route const & route)
{

    std::cout << RED << "GETTING FILE PATH--- " << EOC << std::endl;
    if (route.getRouteType() == Route::FILE)
    	std::cout << RED << "FILE PATH -- IS FILE" << EOC << std::endl;
      return route.getRoot() + "/" + route.getPath();
    try
    {
        string root, index;
        root = route.getRoot().empty() ? server.getRoot() : route.getRoot();
        index = tryFiles(server, route, root);
        return root + "/" + index;
    } catch (ServerException & e)
    {				//Revisar este status también
        if (request->getStatusCode() == STATUS_403 && (route.getAllowListing() || server.getAllowListing())  \
            && request->getMethod() != 'p')
        {
            if (request->getMethod() == 'd')
                return route.getRoot() + "/";
            vector<string> files = getFilesInDirectory(request->getPath());
            return (isListing = true, body = generateHtmlListing(files), "");
        }
        throw ServerException(request->getStatusCode());
    }
}



void Response::processUrlEncodedBody(const string& body)
{
    std::map<string, string> queryStrings;
    vector<string> params = ft_split(body, "&");

    for (vector<string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        vector<string> param = ft_split(*it, "=");
        if (param.size() == 2)
            queryStrings[param[0]] = param[1];
    }
}

void Response::processFileUpload(std::istringstream& ss, const string& line, Route const & route)
{
    int len = line.find("\"", line.find("filename") + 10) - line.find("filename") - 10;
    string filename = route.getUploadDir() + "/" + line.substr(line.find("filename") + 10, len);
    std::ofstream file(filename.c_str());
    string content;
    std::getline(ss, content);
    std::getline(ss, content);
    while (std::getline(ss, content))
        file << content << "\n";
}

void Response::processFormField(std::istringstream& ss, const string& line, std::map<string, string>& queryStrings)
{
    int len = line.find("\"", line.find("name") + 6) - line.find("name") - 6;
    string name = line.substr(line.find("name") + 6, len);
    string content;
    std::getline(ss, content);
    std::getline(ss, content);
    std::getline(ss, content, '\0');
    queryStrings[name] = content;
}

void Response::processMultipartFormDataBody(const string& body, Route const & route)
{
    std::map<string, string> queryStrings;
    string boundary = request->getBoundary();
    vector<string> params = ft_split(body, "--" + boundary);

    for (vector<string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {
        std::istringstream ss(*it);
        string line;
        std::getline(ss, line);

        if (line.find("filename") != string::npos)
            processFileUpload(ss, line, route);
        else if (line.find("name") != string::npos)
            processFormField(ss, line, queryStrings);
    }
}

void Response::readBody(Route const & route)
{
    const string& contentType = request->getContentType();
    const string& body = request->getBody();
    
    if (contentType == "application/x-www-form-urlencoded")
        processUrlEncodedBody(body);
    else if (contentType == "multipart/form-data")
        processMultipartFormDataBody(body, route);
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






















/*     catch (std::exception & e)
    {
        std::cout << CYA << "///////////////////////CODE IS 2 : " << code << EOC << std::endl;
        code = STATUS_500;
        std::cout << CYA << "///////////////////////CODE IS 2 : " << code << EOC << std::endl;
        int c = 500;
        std::cout << CYA << "///////////////////////FILEPATH IS 2 : " << server.getErrorPages()[c] << EOC << std::endl;
        readContent(server.getErrorPages()[c], code);
    } */