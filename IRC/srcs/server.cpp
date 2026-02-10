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

#include "../incs/server.hpp"

Server::Server(int port) : _port(port)
{
	setupSocket();
}

Server::~Server()
{
	close(_serverFd);
}

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
	_fds.push_back(pfd);
	
	std::cout << "Server listening on port " << _port << std::endl;
}

void Server::run()
{
	while (true)
	{
		int ret = poll(&_fds[0], _fds.size(), -1);
		if (ret < 0)
			throw std::runtime_error("poll failed");

		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _serverFd)
					acceptClient();
				else
				{
					if (!receiveFromClient(_fds[i].fd))
						i--;
				}
			}
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
	_fds.push_back(pfd);

	_clients.insert(std::make_pair(clientFd, Client(clientFd)));

	std::cout << "Client connected (fd = " << clientFd << " )" << std::endl;
}

bool Server::receiveFromClient(int clientFd)
{
	char buffer[512];
	std::memset(buffer, 0, sizeof(buffer));

	ssize_t bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
	{
		std::cout << "Client disconnected (fd = " << clientFd << ")\n";
		close(clientFd);
		removeFd(clientFd);
		_clients.erase(clientFd);
		return (false);
	}

	std::map<int, Client>::iterator it = _clients.find(clientFd);
	if (it == _clients.end())
		return (true); //ne devrait jamais arriver
	Client& client = it->second;

	// Ajout au buffer du client
	client.getBuffer().append(buffer, bytes);

	std::string& buf = client.getBuffer();
	size_t pos;

	// Extraction des messages complets (\r\n)
	while ((pos = buf.find("\r\n")) != std::string::npos)
	{
		std::string message = buf.substr(0, pos);
		buf.erase(0, pos + 2);

		std::cout << "Complete message from fd " << clientFd << ": [" << message << "]" << std::endl;
	}
	return (true);
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
