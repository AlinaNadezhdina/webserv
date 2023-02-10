/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wcollen <wcollen@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/05 15:23:51 by mlarra            #+#    #+#             */
/*   Updated: 2023/02/10 22:51:22 by wcollen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLER_HPP
# define HANDLER_HPP

// # define RECV_SIZE 2048
# include "webserv.hpp"

class Server;
class Client;
// class Response;

class Handler
{
private:
	std::vector<Server>*	_servers;
	std::vector<Client *>	_clients;
	// fd_set					_fdSet;
	fd_set					_fdReadSave;
	fd_set					_fdRead;
	fd_set					_fdWriteSave;
	fd_set					_fdWrite;
	// std::vector<int>		_fds;
	int						_maxFd;
	bool					_makeBound;
	std::string				makeEndBoundary(std::string request);
public:
	Handler(std::vector<Server> *);
	~Handler();
	void	initFds();
	void	serverRun();
	void	process(Client *);
	void	processChunk(Client *);
	void	clean();

};


#endif
