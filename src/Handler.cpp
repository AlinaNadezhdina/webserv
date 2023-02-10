/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wcollen <wcollen@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/05 15:24:49 by mlarra            #+#    #+#             */
/*   Updated: 2023/02/10 22:52:14 by wcollen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Handler.hpp"

Handler::Handler(std::vector<Server> * enterServers): _servers(enterServers)
{
}

Handler::~Handler()
{
}

void	Handler::initFds()
{
	FD_ZERO(&_fdReadSave);
	FD_ZERO(&_fdWriteSave);
	_maxFd = 0;
	for (std::size_t i = 0; i < _servers->size(); i++)
	{
		(*_servers)[i].initSocket();
		FD_SET((*_servers)[i].getSocketFd(), &_fdReadSave);
		if ((*_servers)[i].getSocketFd() > _maxFd)
			_maxFd = (*_servers)[i].getSocketFd();
	}
	if (_maxFd == 0)
		exitError("Could not init Fds in Handler");
}

void	Handler::processChunk(Client *client)
{
	std::string	head;
	std::string	chunks;
	std::string	subChunk;
	std::string	body;
	int			chunkSize;
	size_t		i;

	head = client->request.substr(0, client->request.find("\r\n\r\n"));
	chunks = client->request.substr(client->request.find("\r\n\r\n") + 4, client->request.size() - 1);
	subChunk = chunks.substr(0, 100);
	body = "";
	chunkSize = strtol(subChunk.c_str(), NULL, 10);
	i = 0;
	while (chunkSize)
	{
		i = chunks.find("\r\n", i) + 2;
		body += chunks.substr(i, chunkSize);
		i += chunkSize + 2;
		subChunk = chunks.substr(i, 100);
		chunkSize = strtol(subChunk.c_str(), NULL, 16);
	}
	client->request = head + "\r\n\r\n" + body + "\r\n\r\n";
}

void	Handler::process(Client *client)
{

	if (client->request.find("Transfer-Encoding: chunked") != std::string::npos &&
		client->request.find("Transfer-Encoding: chunked") < client->request.find("\r\n\r\n"))
		processChunk(client);
	else if (client->request != "")
	{
		Request	request(client->request);
		if (request.getRet() != 200)
			request.setMethod("GET");
		ResponseConfig responseConf(client->getServerRef(),  request);
		
		Response		response;

		response.call(request, responseConf);

		client->setResponse(response.getResponse());
	}
}

std::string	Handler::makeEndBoundary(std::string request)
{
	std::string	endBound = "";
	std::string	preBoundary = "";
	std::string	boundary = "";
	size_t		i;

	if (request.find("Content-Type") != std::string::npos)
	{
		preBoundary = request.substr(request.find("boundary=") + 9);
		preBoundary = preBoundary.substr(preBoundary.find_first_not_of('-'));
		i = preBoundary.find_first_of("\r\n");
		boundary = preBoundary.substr(0, i);
		endBound = boundary + "--";
		_makeBound = true;
	}
	return (endBound);
}

void	Handler::serverRun()
{
	int				ret = 0;
	char			*buffer = (char *)malloc(10000001);
	int				fdClient;
	struct timeval	timeout;
	std::string		endBound;

	while (true)
	{
		_fdRead = _fdReadSave;
		_fdWrite = _fdWriteSave;
		select(_maxFd + 1, &_fdRead, &_fdWrite, 0, 0);
		//проход по серверам
		for (std::size_t i = 0; i < _servers->size(); i++)
		{
			int	serverFd = (*_servers)[i].getSocketFd();

			if (FD_ISSET(serverFd, &_fdRead))
			{
				Client *client = new Client(serverFd, (*_servers)[i]);
				client->acceptClient();
				FD_SET(client->getFd(), &_fdReadSave);
				_clients.push_back(client);
				if (client->getFd() > _maxFd)
					_maxFd = client->getFd();
			}
		}
//проход по читающим fd
		for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
		{
			fdClient = (*it)->getFd();
			if (FD_ISSET(fdClient, &_fdRead))
			{
				if ((ret = recv(fdClient, buffer, RECV_SIZE - 1, 0)) > 0)
				{
					buffer[ret] = 0;
					(*it)->request += buffer;
					memset(buffer, 0, RECV_SIZE);
				}
				else if (ret == 0)
				{
					FD_CLR(fdClient, &_fdReadSave);
					FD_CLR(fdClient, &_fdWriteSave);
					delete (*it);
					_clients.erase(it);
					break;
				}
				size_t	pos = 0;
				size_t	bodySize = 0;
				bool	isContentLength = false;
				if ((*it)->request.find("\r\n\r\n") != std::string::npos)
				{
					if ((bodySize = (*it)->request.find("Content-Length")) != std::string::npos)
					{
						isContentLength = true;
						bodySize = strtoul((*it)->request.substr(bodySize + 15, (*it)->request.find("\r\n", bodySize) - bodySize).c_str(), 0, 0);
					}
					if ((*it)->request.substr(0, 5).find("POST") != std::string::npos)
					{
						if ((isContentLength && ((*it)->request.substr(pos + 4).size() >= bodySize)) ||
							// ((*it)->request.substr(pos + 4).find("\r\n\r\n") != std::string::npos))
							(*it)->request.find(endBound) != std::string::npos)
						{
							FD_SET(fdClient, &_fdWriteSave);
							FD_CLR(fdClient, &_fdRead);
						}
						else
							break;
					}
					FD_SET(fdClient, &_fdWriteSave);
					FD_CLR(fdClient, &_fdReadSave);
				}
				else
					break;
				process(*it);
			}

//проход по пишущим fd
			if (FD_ISSET(fdClient, &_fdWrite))
			{
				ret = send(fdClient, (*it)->getResponse().c_str(), (*it)->getResponse().size(), 0);
				if (ret <= 0)
				{
					FD_CLR(fdClient, &_fdWriteSave);
					FD_CLR(fdClient, &_fdReadSave);
					delete *it;
					_clients.erase(it);
					break;
				}
				if ((unsigned long)ret < (*it)->getResponse().length())
				{
					(*it)->setResponse((*it)->getResponse().substr(ret));
				}
				else
				{
					FD_CLR(fdClient, &_fdWriteSave);
					(*it)->setResponse("");
					delete *it;
					_clients.erase(it);
					break;
				}
			}
			// check timeout
			memset(&timeout, 0, sizeof(timeout));
			gettimeofday(&timeout, 0);
			if ((FD_ISSET(fdClient, &_fdRead)) && (*it)->getTime() - timeout.tv_sec > 10)
			{
				FD_CLR(fdClient, &_fdReadSave);
				FD_CLR(fdClient, &_fdWriteSave);
				delete *it;
				_clients.erase(it);
				break;
			}
		}
	}
}

void  Handler::clean()
{
  delete _servers;

  //можно так удалять
  // for ( std::vector<Client *>::iterator it = _clients.begin() ; it != _clients.end() ; it++ )
  //   delete *it;s

  //а можно так
  std::for_each( _clients.begin(), _clients.end(), std::default_delete<Client>() );
}