/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:08:26 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:09:26 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleJoin(Client& client, const Command& cmd)
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
	if (_channels.find(channelName) == _channels.end()) {
		_channels.insert(std::make_pair(channelName, Channel(channelName)));
	}
	Channel& channel = _channels[channelName];
	if(channel.hasMember(client.getFd()))
		return;

	if(channel.isInviteOnly()) //if #chan +i
	{
		if(!channel.isInvited(client.getFd())) {
			sendError(client, "473", channelName + " :Cannot join channel (+i)");
			return;
		}
		channel.uninvite(client.getFd());
	}
	if (channel.isFull())
	{
		sendError(client, "471", channelName + " :Cannot join channel (+l)");
		return;
	}
	if (channel.hasKey())
	{
		std::string key = (cmd.params.size() >= 2) ? cmd.params[1] : "";
		if (!channel.checkKey(key))
		{
			sendError(client, "475", channelName + " :Cannot join channel (+k)");
			return;
		}
	}
	channel.addMember(client.getFd());
	if (channel.memberCount() == 1)
		channel.addModerator(client.getFd());
	
	std::string msg = ":" + client.getNick() + "!" + client.getUser() + "@localhost JOIN " + channelName + "\r\n";
	broadcastToChannel(channel, msg, -1);
	sendNames(client, channel);
}


// A JOIN #chan  			 B JOIN #chan 
// JOIN #chan
