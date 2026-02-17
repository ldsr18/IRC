/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   broadcast.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:25:37 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:26:29 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::broadcastToChannel(Channel& channel, const std::string& msg, int exceptFd)
{
	const std::set<int>& mbrs = channel.members();

	for (std::set<int>::const_iterator it = mbrs.begin(); it != mbrs.end(); ++it)
	{
		int fd = *it;

		if (fd == exceptFd)
			continue;

		send(fd, msg.c_str(), msg.size(), 0);
	}
}
