/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:04:44 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:06:03 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handlePass(Client& client, const Command& cmd)
{
	if (client.passAccepted())
	{
		sendError(client, "462", ":You may not reregister");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "461", "PASS :Not enough parameters");
		return;
	}
	if (cmd.params[0] != _password)
	{
		sendError(client, "464", ":Password incorrect");
		return;
	}
	client.setPassAccepted(true);
}
