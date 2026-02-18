/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:22:36 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:22:58 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

bool Server::handleCommand(Client& client, const Command& cmd)
{
	if 		(cmd.name == "PASS")
		handlePass(client, cmd);
	else if (cmd.name == "NICK")
		handleNick(client, cmd);
	else if (cmd.name == "USER")
		handleUser(client, cmd);
	else if (cmd.name == "JOIN")
		handleJoin(client, cmd);
	else if (cmd.name == "INVITE")
		handleInvite(client, cmd);
	else if (cmd.name == "KICK")
		handleKick(client, cmd);
	else if (cmd.name == "TOPIC")
		handleTopic(client, cmd);
	else if (cmd.name == "MODE")
		handleMode(client, cmd);
	else if (cmd.name == "PRIVMSG")
		handlePrivMSG(client, cmd);
	else if (cmd.name == "PING")
		handlePing(client, cmd);
	else if (cmd.name == "PART")
		handlePart(client, cmd);
	else if (cmd.name == "QUIT") {
		handleQuit(client);
		return false;
	}
	else
		sendError(client, "421", cmd.name + " :Unknown command");

	if (client.isRegistered() && !client.hasWelcomed())
	{
		sendWelcome(client);
		client.setWelcomed(true);
	}
	return true;
}
