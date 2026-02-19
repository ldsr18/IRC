/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 09:19:17 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/06 09:19:17 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password)
{
	setupSocket();
}

Server::~Server()
{
	std::cout << "\nShutting down server..." << std::endl;
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
	_fds.clear();
	_clients.clear();
	_channels.clear();
	std::cout << "Server stopped cleanly." << std::endl;
}
