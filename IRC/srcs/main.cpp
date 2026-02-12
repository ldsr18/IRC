/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 08:43:54 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/06 08:43:54 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}

	int port = std::atoi(argv[1]);
	std::string password = argv[2];
	
	Server serv(port, password);
	serv.run();
}