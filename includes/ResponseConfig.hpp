/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlarra <mlarra@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/28 16:29:05 by mlarra            #+#    #+#             */
/*   Updated: 2023/01/09 14:33:41 by mlarra           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPONSECONFIG_HPP
# define RESPONSECONFIG_HPP

# include "webserv.hpp"

class ResponseConfig
{
private:
	std::string		_contentLocation;
	std::string		_path;
	std::string		_error_page;// ????
	unsigned long	body_size; // max size for the client body, defaults to 8 000??
	std::string		_lang;

	std::vector<std::string>			_extension_cgi;
	std::string							_exec_cgi;
	Server			&_server;
	Request			&_request;

public:
	ResponseConfig(Server &server, Request &request);
	~ResponseConfig();
	Server				&getServer() const;
	const std::string	&getPath() const;
	std::string			removeSlashes(const std::string &);

	std::vector<std::string>	makeCgiVector();
};




# endif
