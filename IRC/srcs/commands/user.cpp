/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:07:27 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:08:06 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleUser(Client& client, const Command& cmd)
{
	if (!client.passAccepted())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if (cmd.params.size() < 4)
	{
		sendError(client, "461", "User :Not enough parameters");
		return;
	}
	client.setUser(cmd.params[0]);
}
