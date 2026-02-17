/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:27:01 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:39:11 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void Server::sendMode(Client& client, std::string channelName, std::string&  modes, std::string&  params)
{
	std::string output = ":ircserv 324 " + client.getNick() + " " + channelName + " " + modes;
	if(!params.empty())
		output += " " + params;
	output += "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}

//:ircserv 332 <nick> <#chan> :<topic>
void Server::sendTopic(Client& client, Channel& channel, std::string const& code, const std::string& message)
{
	std::string output 		= ":ircserv " + code + " " + client.getNick() + " " 
							+ channel.name() + message + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}


void Server::sendReply(Client& client, Client& target, Channel& channel, std::string const& code)
{
	std::string output 		= ":ircserv " + code + " " + client.getNick() + " " 
							+ target.getNick() + " " + channel.name() + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}

void Server::sendInvit(Client& client, Client& target, Channel& channel)
{
	std::string output = 	":" + client.getNick() + "!" + client.getUser()
							+ "@localhost INVITE "+ target.getNick() + " " 
							+ channel.name() + "\r\n";
	send(target.getFd(), output.c_str(), output.size(), 0);
}

void Server::sendError(Client& client, const std::string& code, const std::string& message)
{
	std::string nick = client.getNick().empty() ? "*" : client.getNick();
	std::string output = ":ircserv " + code + " " + nick + " " + message + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}


void Server::sendWelcome(Client& client)
{
	std::string output = ":ircserv 001 " + client.getNick() + " :Welcome to our ircserv " + client.getNick() + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}

void Server::sendNames(Client& client, Channel& channel)
{
	std::string list;
	const std::set<int>& members = channel.members();

	for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		Client& member = _clients[*it];
		if (channel.isModerator(*it))
			list += "@";
		list += member.getNick() + " ";
	}
	
	if (!list.empty())
		list.erase(list.size() - 1);
	
	std::string reply353 = ":ircserv 353 " + client.getNick() + " = " + channel.name() + " :" + list + "\r\n";
	send(client.getFd(), reply353.c_str(), reply353.size(), 0);
	std::string reply366 = ":ircserv 366 " + client.getNick() + " " + channel.name() + " :End of /NAMES list.\r\n";
	send(client.getFd(), reply366.c_str(), reply366.size(), 0);
}
