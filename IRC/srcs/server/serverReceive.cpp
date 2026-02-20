/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverReceive.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:21:13 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/20 09:59:04 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

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
		return (true);
	Client& client = it->second;
	client.getBuffer().append(buffer, bytes);
	if (client.getBuffer().size() > 5120)
	{
		std::cout << "Limit exceeded for fd " << clientFd << ". Kicking client." << std::endl;
		handleQuit(client);
		return false;
	}

	std::string& buf = client.getBuffer();
	size_t pos;

	/*Extraction des messages complets (\r\n)
	debug
	std::cout << "Complete message from fd " << clientFd << ": [" << message << "]" << std::endl;*/
	while ((pos = buf.find("\r\n")) != std::string::npos)
	{
		std::string message = buf.substr(0, pos);
		buf.erase(0, pos + 2);

		Command cmd = parseCommand(message);
		if (!cmd.name.empty())
			if(!handleCommand(client, cmd))
				return false;
	}
	return (true);
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
