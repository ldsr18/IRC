/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:10:59 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:11:24 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleInvite(Client& client, const Command& cmd)
{
	if (!client.isRegistered()) {
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 2) {
		sendError(client, "461", "INVITE :Not enough parameters");
		return;
	}

	std::string const& targetNick  = cmd.params[0];
	std::string const& channelName = cmd.params[1];
	Channel* channel = findChannelByName(channelName);
	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if (!channel->hasMember(client.getFd())) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	if (!channel->isModerator(client.getFd())) {
		sendError(client, "482", channel->name() + " :You're not channel operator");
		return;
	}

	Client* target = findClientByNick(targetNick);
	if (!target) {
		sendError(client, "401", targetNick + " :No such nick");
		return;
	}

	if (channel->hasMember(target->getFd())) {
		sendError(client, "443", targetNick + " " + channelName + " :is already on channel");
		return;
	}
	if (channel->isInvited(target->getFd())) {
		sendError(client, "443", targetNick + " " + channelName + " :is already invited");
		return;
	}
	channel->invite(target->getFd());
	sendInvit(client, *target, *channel);
	sendReply(client, *target, *channel, "341");
}
