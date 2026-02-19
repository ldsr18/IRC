/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:09:44 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/19 17:47:30 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleMessage(Client& client, const Command& cmd, bool isNotice)
{
	if (!client.isRegistered())
	{
		if (!isNotice)
			sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		if (!isNotice)
			sendError(client, "411", ":No recipient given (PRIVMSG)");
		return;
	}
	if (cmd.params.size() < 2)
	{
		if (!isNotice)
			sendError(client, "412", ":No text to send");
		return;
	}
	std::string target = cmd.params[0];
	std::string message = cmd.params[1];
	std::string commandName = isNotice ? "NOTICE" : "PRIVMSG";
	if (target[0] == '#')
	{
		Channel* channel = findChannelByName(target);
		if (!channel)
		{
			if (!isNotice)
				sendError(client, "403", target + " :No such channel");
			return;
		}
		if (!channel->hasMember(client.getFd()))
		{
			if (!isNotice)
				sendError(client, "404", target + " :Cannot send to channel");
			return;
		}
		std::string output = ":" + client.getNick() + "!" + client.getUser() + "@localhost " + commandName + " " + target + " :" + message + "\r\n";
		broadcastToChannel(*channel, output, client.getFd());
	}
	else
	{
		Client* targetClient = findClientByNick(target);
		if (!targetClient)
		{
			if (!isNotice)
				sendError(client, "401", target + " :No such nick/channel");
			return;
		}
		std::string output = ":" + client.getNick() + "!" + client.getUser() + "@localhost " + commandName + " " + target + " :" + message + "\r\n";
		send(targetClient->getFd(), output.c_str(), output.size(), 0);
	}
}
