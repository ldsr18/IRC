/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 08:45:12 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/06 08:45:12 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "client.hpp"

#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>

#include <unistd.h> //close
#include <fcntl.h>
#include <sys/socket.h> //socket, bind, listen...
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //htons
#include <poll.h> //poll, pollfd

class Server
{
	private:
			int _port;
			int _serverFd; //fd du serveur = le socket qui écoute
			std::vector<struct pollfd> _fds;
			std::map<int, Client> _clients;

			void setupSocket(); //crée et prépare lee serveur
			void acceptClient();
			bool receiveFromClient(int clientFd);
			void removeFd(int fd);

	public:
			Server(int port);
			~Server();

			void run();
};

#endif