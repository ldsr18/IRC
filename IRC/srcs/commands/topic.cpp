/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:11:54 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:12:34 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleTopic(Client& client, const Command& cmd)
{
	if(!client.isRegistered())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if(cmd.params.size() < 1)
	{
		sendError(client, "461", "TOPIC :Not enough parameters");
		return;
	}
	std::string const& channelName = cmd.params[0];
	Channel *channel = findChannelByName(channelName);
	if(!channel)
	{
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if(!channel->hasMember(client.getFd()))
	{
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	if(cmd.params.size() == 1)
	{
		if(channel->topic() == "")
			sendTopic(client, *channel, "331", " :No topic is set");
		else
			sendTopic(client, *channel, "332", " :" + channel->topic());
		return;
	}
	if( channel->isTopicRestricted() && !channel->isModerator(client.getFd()))
	{
		sendError(client, "482", channelName + " :You're not channel operator");
		return;
	}
	std::string const& newTopic = cmd.params[1];
	channel->setTopic(newTopic);

	std::string msg = ":" + client.getNick() + "!" + client.getUser()
					+ "@localhost TOPIC " + channel->name()
					+ " :" + newTopic + "\r\n";
	broadcastToChannel(*channel, msg, -1);
}