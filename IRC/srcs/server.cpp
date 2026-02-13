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
#include "../incs/client.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password)
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

		//debug, à supprimer a la fin !!!!!!!!!!
		std::cout << "Complete message from fd " << clientFd << ": [" << message << "]" << std::endl;
		
		Command cmd = parseCommand(message);
		if (!cmd.name.empty())
			handleCommand(client, cmd);
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

Command Server::parseCommand(const std::string& line)
{
	Command cmd;

	size_t pos = line.find(" :");
	std::string commandPart;
	std::string messagePart;

	if (pos != std::string::npos)
	{
		commandPart = line.substr(0, pos);
		messagePart = line.substr(pos + 2);
	}
	else
		commandPart = line;

	std::istringstream iss(commandPart);
	std::string word;

	if (iss >> word)
		cmd.name = word;
	while (iss >> word)
		cmd.params.push_back(word);

	if (!messagePart.empty())
		cmd.params.push_back(messagePart);

	return (cmd);
}

void Server::handleCommand(Client& client, const Command& cmd)
{
	if (cmd.name == "PASS")
		handlePass(client, cmd);
	else if (cmd.name == "NICK")
		handleNick(client, cmd);
	else if (cmd.name == "USER")
		handleUser(client, cmd);
	else if (cmd.name == "JOIN")
		handleJoin(client, cmd);
	else
		sendError(client, "421", cmd.name + ":Unknown command");

	if (client.isRegistered())
		sendWelcome(client);
}

void Server::handlePass(Client& client, const Command& cmd)
{
	if (client.passAccepted())
	{
		sendError(client, "462", ":You may not reregister");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "461", "PASS :Not enough parameters");
		return;
	}
	if (cmd.params[0] != _password)
	{
		sendError(client, "464", ":Password incorrect");
		return;
	}
	client.setPassAccepted(true);
}

void Server::handleNick(Client& client, const Command& cmd)
{
	if (!client.passAccepted())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "431", "*:No nickname given");
		return;
	}
	std::string nickname = cmd.params[0];
	if (nicknameExists(nickname))
	{
		sendError(client, "433", "* " + nickname + " :Nickname is already use");
		return;
	}
	client.setNick(nickname);
}

void Server::handleUser(Client& client, const Command& cmd)
{
	if (!client.passAccepted())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 4)
	{
		sendError(client, "461", "User :Not enough parameters");
		return;
	}
	client.setUser(cmd.params[0]);
}

void Server::handleJoin(Client& client, Command& cmd)
{
	 if (!client.isRegistered())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "461", "JOIN :Not enough parameters");
		return;
	}
	std::string channelName = cmd.params[0];
	if (channelName[0] != '#')
	{
		sendError(client, "476", channelName + " :Bad Channel Mask");
		return;
	}
	if (_channels.find(channelName) == _channel.end())
		_channels.insert(std::make_pair(channelName, Channel(channelName)));
	Channel& channel = _channels[channelName];

	std::string msg = ":" + client.getNick() + "!" + client.getUser() + "@localhost JOIN " + channelName + "\r\n";

	broadcastToChannel(channel, msg, -1);
}

bool Server::nicknameExists(const std::string& nick)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNick() == nick)
			return (true);
	}
	return (false);
}

void Server::sendError(Client& client, const std::string& code, const std::string& message)
{
	std::string output = ":ircserv " + code + " * " + message + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}

void Server::sendWelcome(Client& client)
{
	std::string output = ":ircserv 001 " + client.getNick() + ":Welcome to our ircserv " + client.getNick() + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}

void Server::broadcastToChannel(Channel& channel, td::string const& msg, int exceptFd)
{
	const std::set<int>& mem = channel.members();
	for (std::set<int>::const_iterator it = mem.begin(); it != mem.end(); ++it)
	{
		int fd = *it;
		if (fd == exceptFd) continue;
		send(fd, msg.c_str(), msg.size(), 0);
	}
}
