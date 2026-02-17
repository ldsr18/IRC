/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverReceive.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:21:13 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:21:52 by jdecarro         ###   ########.fr       */
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
