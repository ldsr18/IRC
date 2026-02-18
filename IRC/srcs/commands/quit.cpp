/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:15:51 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:16:01 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleQuit(Client& client, const Command& cmd)
{
	if (cmd.params.empty())
		return;

	std::string reply = "PONG :" + cmd.params[0] + "\r\n";
	send(client.getFd(), reply.c_str(), reply.size(), 0);
}
