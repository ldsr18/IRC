/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:19:48 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/19 15:25:05 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::setupSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw std::runtime_error("Socket failed!");

	int opt = 1;
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);
	
	if (bind(_serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind failed");
	if (listen(_serverFd, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");
		
	// non-bloquant (OBLIGATOIRE pour ft_irc)
	fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	
	pollfd pfd;
	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);
	
	std::cout << "Server listening on port " << _port << std::endl;
}

void Server::run()
{
	while (server_running)
	{
		int ret = poll(&_fds[0], _fds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll failed");
		}
		for (size_t i = 0; i < _fds.size(); )
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _serverFd)
					acceptClient();
				else
				{
					if (!receiveFromClient(_fds[i].fd))
						continue;
				}
			}
			i++;
		}
	}
}

void Server::acceptClient()
{
	sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);

	int clientFd = accept(_serverFd, (sockaddr*)&clientAddr, &len);
	if (clientFd < 0)
		return;

	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_fds.push_back(pfd);

	_clients.insert(std::make_pair(clientFd, Client(clientFd)));

	std::cout << "Client connected (fd = " << clientFd << ")" << std::endl;
}

void Server::removeFd(int fd)
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			return;
		}
	}
}
