/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:09:44 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:10:09 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handlePrivMSG(Client& client, const Command& cmd)
{
	if (!client.isRegistered())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "411", ":No recipient given (PRIVMSG)");
		return;
	}
	if (cmd.params.size() < 2)
	{
		sendError(client, "412", ":No text to send");
		return;
	}
	std::string target = cmd.params[0];
	std::string message = cmd.params[1];

	if (target[0] == '#')
	{
		Channel* channel = findChannelByName(target);
		if (!channel)
		{
			sendError(client, "403", target + " :No such channel");
			return;
		}
		if (!channel->hasMember(client.getFd()))
		{
			sendError(client, "404", target + " :Cannot send to channel");
			return;
		}
		std::string output = ":" + client.getNick() + "!" + client.getUser() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
		broadcastToChannel(*channel, output, client.getFd());
	}
	else
	{
		Client* targetClient = findClientByNick(target);
		if (!targetClient)
		{
			sendError(client, "401", target + " :No such nick/channel");
			return;
		}
		std::string output = ":" + client.getNick() + "!" + client.getUser() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
		send(targetClient->getFd(), output.c_str(), output.size(), 0);
	}
}
