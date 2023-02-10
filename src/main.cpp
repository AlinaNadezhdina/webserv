/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wcollen <wcollen@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 12:25:05 by mlarra            #+#    #+#             */
/*   Updated: 2023/02/10 22:52:28 by wcollen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

int	main(int ac, char **av)
{
	const char  *fileName;

	if (ac > 2)
		exitError("Wrong numbers of arguments");
	if (ac == 1)
		fileName = "config.conf";
	else
		fileName = av[1];

	Config	config(fileName);
	config.parse();
	Handler	handler(config.getConfig());
	handler.initFds();
	handler.serverRun();
	handler.clean();
}
