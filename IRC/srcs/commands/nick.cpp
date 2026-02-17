/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:06:15 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:06:41 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleNick(Client& client, const Command& cmd)
{
	if (!client.passAccepted())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "431", ":No nickname given");
		return;
	}
	std::string nickname = cmd.params[0];
	if (nicknameExists(nickname))
	{
		sendError(client, "433", nickname + " :Nickname is already in use");
		return;
	}
	client.setNick(nickname);
}
