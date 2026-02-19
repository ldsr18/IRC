/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:15:51 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/19 10:55:04 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::handleQuit(Client& client)
{
	std::map<std::string, Channel>::iterator it;
	std::cout << "Client disconnected (fd = " << client.getFd() << ")\n";
	for(it = _channels.begin(); it != _channels.end();) {
		std::map<std::string, Channel>::iterator next = it;
		next++;
		if(it->second.hasMember(client.getFd())) {
			std::string msg;
			// ":" + NICK + "!" + USER + "@" + "localhost" + "QUIT :Client Quit"
			msg = ":" + client.getNick() + "!" + client.getUser() + "@localhost QUIT :Client Quit\r\n";
			broadcastToChannel(it->second, msg, -1);
			it->second.removeMember(client.getFd());
			if(!it->second.memberCount()) {
				_channels.erase(it->first);
			}
		}
		it = next;
	}
	std::string msg = "ERROR :Closing Link: localhost (Client Quit)\r\n";
	send(client.getFd(), msg.c_str(), msg.size(), 0);
	close(client.getFd());
	removeFd(client.getFd());
	_clients.erase(client.getFd());
}
