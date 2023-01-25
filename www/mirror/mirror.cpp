/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mirror.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlarra <mlarra@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 21:24:45 by mlarra            #+#    #+#             */
/*   Updated: 2023/01/25 10:57:20 by mlarra           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

int	main(int ac, char **av)
{
	int			i;
	int			j;
	int			len;
	std::string	rez;

	i = 1;
	while (av[i])
	{
		j = strlen(av[i]);
		while (j >= 0)
		{
			rez += av[i][j];
			j--;
		}
		rez += " ";
		i++;
	}
std::cout << "rezult: " << rez << std::endl;
	return (0);
}