/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlarra <mlarra@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 12:49:54 by mlarra            #+#    #+#             */
/*   Updated: 2023/01/31 14:12:25 by mlarra           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

// Client::Client(int enterServerFd, int servIndex): _fd(-1), _serverFd(enterServerFd), _response(""), _servIndex(servIndex)
Client::Client(int enterServerFd, Server & enterServer): _fd(-1), _serverFd(enterServerFd), _serverRef(enterServer)//, _servIndex(servIndex)
{
	// (void)_servIndex;
	memset(&_clientAddrIn, 0, sizeof(_clientAddrIn));
	// memset(&_timeout, 0, sizeof(_timeout));
	// gettimeofday(&_timeout, 0);
	this->request = "";
// _response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\r\n\r\nHello world!";
	
}

Client::~Client()
{
}

int	Client::getFd() const
{
	return (_fd);
}

int	Client::getServerFd() const
{
	return (_serverFd);
}

void	Client::acceptClient()
{
	int	addrLen;

	addrLen = sizeof(sockaddr_in);
	_fd = accept(_serverFd,(struct sockaddr *)&_clientAddrIn, (socklen_t *)&addrLen);
	if (_fd < 0)
		exitError("Accept");
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	_ipAddress = inet_ntoa(_clientAddrIn.sin_addr);
}

const std::string	&Client::getResponse() const
{
	return (_response);
}

const std::string	&Client::getIpAddress() const
{
	return (_ipAddress);
}

const Server	&Client::getServerRef() const
{
	return (_serverRef);
}

void	Client::setResponse(std::string enterResponse)
{
	_response = enterResponse;
}
