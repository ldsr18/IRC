/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:10:22 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:10:46 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void	Server::handleKick(Client &client, const Command& cmd) {
	

	if(!client.isRegistered()) {
		sendError(client, "451", ":You have not registered");
		return;
	}
	if(cmd.params.size() < 2) {
		sendError(client, "461", "KICK :Not enough parameters");
		return;
	}
	std::string const& channelName = cmd.params[0];
	std::string const& targetNick = cmd.params[1];
	Channel *channel = findChannelByName(channelName);
	if(!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if(!channel->hasMember(client.getFd())) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	if(!channel->isModerator(client.getFd())) {
		sendError(client, "482", channelName + " :You're not channel operator");
		return;
	}
	Client *clientTarget = findClientByNick(targetNick);
	if(!clientTarget) {
		sendError(client, "401", targetNick + " :No such nick");
		return;
	}
	if(!channel->hasMember(clientTarget->getFd())) {
		sendError(client, "441", targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}
	std::string reason = " :Kicked!";
	if(cmd.params.size() > 2)
		reason = " :" + cmd.params[2];
	std::string kickMsg 	= ":" + client.getNick() + "!" + client.getUser() + "@localhost KICK "
							+ channel->name() + " " + clientTarget->getNick() +  reason + "\r\n"; 
	broadcastToChannel(*channel, kickMsg, -1);
	channel->removeMember(clientTarget->getFd());
	if(channel->empty())
		_channels.erase(channelName);
}
