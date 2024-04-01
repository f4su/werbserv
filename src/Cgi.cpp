#include "../inc/Cgi.hpp"

Cgi::Cgi(): responseBody(""), responseHeaders()  
{
}

Cgi::Cgi(Route const & cgiRoute, std::string const & filename, URI const & req)
{
    this->cgiRoute = cgiRoute;
	this->filename = filename;
	this->env = getEnv(req, filename);
	this->envp = mapToArray(this->env);
	executeCgi(req);
}

Cgi::Cgi(Cgi const& other)
{
	*this = other;
}

Cgi::~Cgi()
{
	for (int i = 0; this->envp[i]; i++)
		delete [] this->envp[i];
	delete [] this->envp;
}

Cgi& Cgi::operator=(Cgi const& other)
{
	if (this != &other)
	{
		this->cgiRoute = other.cgiRoute;
		this->filename = other.filename;
	}
	return (*this);
}

std::map<std::string, std::string> Cgi::getEnv(URI const & req, std::string const & filename)
{
	std::map<std::string, std::string> env;
	env["SERVER_SOFTWARE"] = "webserv/1.0";
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["REDIRECT_STATUS"] = "1";
	env["SERVER_PROTOCOL"] = req.getVersion();
	env["SERVER_PORT"] = toString(req.getPort());
	env["REQUEST_METHOD"] = req.getMethod2();
	env["PATH_INFO"] = filename;
	env["PATH_TRANSLATED"] = filename;
	env["QUERY_STRING"] = getQuery(req.getUri()); //req.getQuery();
	env["REMOTE_HOST"] = req.getHost();
	if (req.getRawBody().size() > 0)
		env["CONTENT_LENGTH"] = toString(req.getRawBody().size());
	if (req.getHeaders().count("CONTENT-TYPE") > 0)
		env["CONTENT_TYPE"] = req.getHeaders()["CONTENT-TYPE"].at(0);
	return (env);
}

char	**Cgi::mapToArray(std::map <std::string, std::string> const & map)
{
	char **array = new char*[map.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); it++)
	{
		std::string str = it->first + "=" + it->second;
		array[i] = new char[str.size() + 1];
		strcpy(array[i], str.c_str());
		i++;
	}
	array[i] = NULL;
	return (array);
}

std::string Cgi::getQuery(std::string const & uri)
{
	std::string query;
	size_t pos = uri.find("?");
	if (pos != std::string::npos)
		query = uri.substr(pos + 1);
	return (query);
}

std::string Cgi::getResponseBody()
{
	return (this->responseBody);
}

std::map<std::string, std::string> Cgi::getResponseHeaders()
{
	return (this->responseHeaders);
}

std::string Cgi::getFileExt(std::string const &path)
{
    std::string ext = path.substr(path.find_last_of("."));
	return (ext);
}

void    Cgi::executeCgi(URI const & req)
{
	std::string cgiPath;
	int fd[2];

	if (pipe(fd) == -1)
		throw ServerException(STATUS_500);
	pid_t pid = fork();
	if (pid == -1)
		throw ServerException(STATUS_500);
	int	status;
	std::string binPath = cgiRoute.getCgi()[getFileExt(filename)];
	if (pid == 0)
	{
		if (dup2(fd[0], 0) == -1 || dup2(fd[1], 1) == -1)
			throw ServerException(STATUS_500);
		close(fd[1]);
		close(fd[0]);
		char *argv[] = {const_cast<char *>(binPath.c_str()), const_cast<char *>(filename.c_str()), NULL};
		execve(binPath.c_str(), argv, envp);
		exit(1);
	}
	else if (pid > 0)
	{
		write(fd[1], req.getRawBody().c_str(), req.getRawBody().size());
		waitpid(pid, &status, 0);
		if (WEXITSTATUS(status) != 0)
			throw ServerException(STATUS_502);
		close(fd[1]);
		char buffer[10240];
		std::string body;
		int ret;
		while ((ret = read(fd[0], buffer, 1023)) > 0)
		{
			buffer[ret] = '\0';
			body += buffer;
		}
		if (body.find("\r\n\r\n") != std::string::npos) {
			std::string headers = body.substr(0, body.find("\r\n\r\n"));
			std::istringstream iss(headers);
			std::string line;
			while (std::getline(iss, line))
			{
				size_t pos = line.find(": ");
				if (pos != std::string::npos)
				{
					std::string key = line.substr(0, pos);
					std::string value = line.substr(pos + 2);
					this->responseHeaders[key] = value;
				}
			}
			body = body.substr(body.find("\r\n\r\n") + 4);
		}
		this->responseBody = body;
		close(fd[0]);
		return;
	}
	else
		throw ServerException(STATUS_500);
}
