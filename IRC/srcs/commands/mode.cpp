/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 11:12:51 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/17 11:36:35 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

std::string toString(size_t n)
{
	std::ostringstream oss;
	oss << n;
	
	return oss.str();
}

void Server::handleMode(Client& client, const Command& cmd)
{
	if(!client.isRegistered())
	{
		sendError(client, "451", ":You have not registered");
		return;
	}
	if(cmd.params.size() < 1)
	{
		sendError(client, "461", "MODE :Not enough parameters");
		return;
	}
	if (cmd.params[0][0] != '#')
    	return;
	std::string channelName = cmd.params[0];
	Channel *channel = findChannelByName(channelName);
	if(!channel)
	{
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if(!channel->hasMember(client.getFd()))
	{
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	// MODE #channel
	if(cmd.params.size() == 1)
	{
		std::string modes = "+";
		std::string params;

		if(channel->isInviteOnly())
			modes += "i";
		if(channel->isTopicRestricted())
			modes += "t";
		if(channel->hasKey())
			modes += "k";
		if(channel->hasLimit())
			modes += "l";
		if(channel->hasLimit())
		{
			if(!params.empty())
				params += " ";
			params += toString(channel->limit());
		}
		sendMode(client, channelName, modes, params);
		return;
	}
	//MODE #channel <args>
	//MODE #t +t` / `MODE #t -t
	std::string modeStr = cmd.params[1];
	if(modeStr[0] != '+' && modeStr[0] != '-')
	{
		sendError(client, "501", ":Unknown MODE flag");
		return;
	}
	if(!channel->isModerator(client.getFd()))
	{
		sendError(client, "482", channelName + " :You're not channel operator");
		return;
	}
	char sign = modeStr[0];
	int paramsCnt = cmd.params.size();
	int paramsIdx = 2;
	for(size_t i = 1; i < modeStr.length(); i++)
	{
		if (modeStr[i] == '+' || modeStr[i] == '-' )
		{
			sign = modeStr[i];
			continue;
		}
		else if	(modeStr[i] == 't')
		{
			
			bool oldValue = channel->isTopicRestricted(); //+t -
			if(sign == '+')
				channel->setTopicRestricted(true);
			else if(sign == '-')
				channel->setTopicRestricted(false);
			if(oldValue != channel->isTopicRestricted())
			{
					std::string msg = 	":" + client.getNick() + "!" + client.getUser()
										+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "t" + "\r\n";
					broadcastToChannel(*channel, msg, -1);
			}
		}
		else if (modeStr[i] == 'i')
		{
			bool oldValue = channel->isInviteOnly();
			if(sign == '+')
				channel->setInviteOnly(true);
			else if(sign == '-')
				channel->setInviteOnly(false);
			if(oldValue != channel->isInviteOnly())
			{
					std::string msg = 	":" + client.getNick() + "!" + client.getUser()
										+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "i" + "\r\n";
					broadcastToChannel(*channel, msg, -1);
			}
		}
		else if (modeStr[i] == 'k')
		{
			bool oldHasKey = channel->hasKey();
			std::string oldKey;
			if (oldHasKey)
				oldKey = channel->getKEy();

			std::string newKey;
			if (sign == '+')
			{
				if (paramsIdx >= paramsCnt)
				{
					sendError(client, "461", "MODE :Not enough parameters");
					return;
				}
				newKey = cmd.params[paramsIdx];
				channel->setKey(newKey);
				paramsIdx++;
			}
			else if (sign == '-')
				channel->clearKey();

			bool changed = false;
			if (sign == '+')
			{
				if (!oldHasKey)
					changed = true;
				else {
					if (oldKey != newKey)
						changed = true;
				}
			}
			else if (sign == '-')
			{
				if (oldHasKey)
					changed = true;
			}

			if (changed)
			{
				std::string msg = ":" + client.getNick() + "!" + client.getUser()
					+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "k\r\n";
				broadcastToChannel(*channel, msg, -1);
			}
		}
		else if (modeStr[i] == 'l')
		{
			bool oldHaslimit = channel->hasLimit();
			size_t oldLimit = 0;
			if(oldHaslimit)
				oldLimit = channel->limit();
			
			std::string newLimits;
			size_t limit = 0;

			if(sign == '+')
			{
				if(paramsIdx >= paramsCnt)
				{
					sendError(client, "461", "MODE :Not enough parameters");
					return;
				}
				newLimits = cmd.params[paramsIdx];
				std::istringstream iss(newLimits);
				char extra;
				if (!(iss >> limit) || iss >> extra || limit == 0)
				{
					sendError(client, "461", "MODE :Invalid limit");
					return;
				}
				channel->setLimit(limit);
				paramsIdx++;
			}
			else if(sign == '-')
				channel->clearLimit();

			bool changed = false;
			if(sign == '+')
			{
				if(!oldHaslimit)
					changed = true;
				else
				{
					if(oldLimit != limit)
						changed = true;
				}
			}
			else if (sign == '-')
			{
				if (oldHaslimit)
					changed = true;
			}

			if (changed)
			{
				std::string msg = ":" + client.getNick() + "!" + client.getUser()
								+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "l";
				if (sign == '+')
					msg += " " + toString(limit);
				msg += "\r\n";
				broadcastToChannel(*channel, msg, -1);
			}
		}
		else if (modeStr[i] == 'o')
		{
			if(paramsIdx >= paramsCnt)
			{
				sendError(client, "461", "MODE :Not enough parameters");
				return;
			}
			std::string targetNick = cmd.params[paramsIdx];
			paramsIdx++;

			Client* clientTarget = findClientByNick(targetNick);
			if(!clientTarget)
			{
				sendError(client, "401", targetNick + " :No such nick");
				return;
			}
			if(!channel->hasMember(clientTarget->getFd()))
			{
				sendError(client, "441", targetNick + " " + channelName + " :They aren't on that channel");
				return;
			}
			bool changed = false;
			if(sign == '+')
				changed = channel->addModerator(clientTarget->getFd());
			else if(sign == '-')
				changed = channel->removeModerator(clientTarget->getFd());
			if(changed)
			{
				std::string msg = 	":" + client.getNick() + "!" + client.getUser()
									+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "o " + targetNick + "\r\n";
				broadcastToChannel(*channel, msg, -1);
			}
		}
		else
		{
			std::string x(1, modeStr[i]);
			sendError(client, "472", x + " :is unknown mode char to me");
			return;
		}
	}
}
