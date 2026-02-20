/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:10:59 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/20 09:57:59 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

// PART #chan
void Server::handlePart(Client& client, const Command& cmd)
{
	if (!client.isRegistered()) {
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 1) { // PART
		sendError(client, "461", "PART :Not enough parameters");
		return;
	}
	std::string const& channelName = cmd.params[0]; 
	Channel* channel = findChannelByName(channelName);
	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if (!channel->hasMember(client.getFd()))
	{
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	std::string msg;
	std::string reason;
	msg = ":" + client.getNick() + "!" + client.getUser() + "@localhost PART " + channel->name();
	if(cmd.params.size() > 1) {
		reason  = cmd.params[1];
		msg += " :" + reason;
	}
	msg += "\r\n";
	broadcastToChannel(*channel, msg, -1);
	channel->removeMember(client.getFd());
	if(!channel->memberCount())
		_channels.erase(channelName);
	
}
