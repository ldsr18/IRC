/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverFind.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:30:14 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:30:36 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

Channel* Server::findChannelByName(const std::string& name) {

	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if(it == _channels.end())
			return NULL;
	return &it->second;
}

Client* Server::findClientByNick(const std::string& nick) {

	std::map<int, Client>::iterator it;
	for(it = _clients.begin(); it != _clients.end(); it++) {
		if(it->second.getNick() == nick)
			return &it->second;
	}
	return NULL;
}

bool Server::nicknameExists(const std::string& nick)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNick() == nick)
			return (true);
	}
	return (false);
}
