/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 09:19:17 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/06 09:19:17 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "client.hpp"

Server::Server(int port, const std::string& password) : _port(port), _serverFd(-1), _password(password)
{
	setupSocket();
}

Server::~Server()
{
	close(_serverFd);
}

void Server::setupSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw std::runtime_error("Socket failed!");

	int opt = 1;
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);
	
	if (bind(_serverFd, (sockaddr*)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("bind failed");
	if (listen(_serverFd, SOMAXCONN) < 0)
		throw std::runtime_error("listen failed");
		
	// non-bloquant (OBLIGATOIRE pour ft_irc)
	fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	
	pollfd pfd;
	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	_fds.push_back(pfd);
	
	std::cout << "Server listening on port " << _port << std::endl;
}

void Server::run()
{
	while (true)
	{
		int ret = poll(&_fds[0], _fds.size(), -1);
		if (ret < 0)
			throw std::runtime_error("poll failed");

		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _serverFd)
					acceptClient();
				else
				{
					if (!receiveFromClient(_fds[i].fd))
						i--;
				}
			}
		}
	}
}

void Server::acceptClient()
{
	sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);

	int clientFd = accept(_serverFd, (sockaddr*)&clientAddr, &len);
	if (clientFd < 0)
		return;

	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	_fds.push_back(pfd);

	_clients.insert(std::make_pair(clientFd, Client(clientFd)));

	std::cout << "Client connected (fd = " << clientFd << " )" << std::endl;
}

bool Server::receiveFromClient(int clientFd)
{
	char buffer[512];
	std::memset(buffer, 0, sizeof(buffer));

	ssize_t bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
	{
		std::cout << "Client disconnected (fd = " << clientFd << ")\n";
		close(clientFd);
		removeFd(clientFd);
		_clients.erase(clientFd);
		return (false);
	}

	std::map<int, Client>::iterator it = _clients.find(clientFd);
	if (it == _clients.end())
		return (true); //ne devrait jamais arriver
	Client& client = it->second;
	// Ajout au buffer du client
	client.getBuffer().append(buffer, bytes);

	std::string& buf = client.getBuffer();
	size_t pos;

	// Extraction des messages complets (\r\n)
	while ((pos = buf.find("\r\n")) != std::string::npos)
	{
		std::string message = buf.substr(0, pos);
		buf.erase(0, pos + 2);

		//debug, à supprimer a la fin !!!!!!!!!!
		std::cout << "Complete message from fd " << clientFd << ": [" << message << "]" << std::endl;
		
		Command cmd = parseCommand(message);
		if (!cmd.name.empty())
			handleCommand(client, cmd);
	}
	return (true);
}

void Server::removeFd(int fd)
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			return;
		}
	}
}

Command Server::parseCommand(const std::string& line)
{
	Command cmd;

	size_t pos = line.find(" :");
	std::string commandPart;
	std::string messagePart;

	if (pos != std::string::npos)
	{
		commandPart = line.substr(0, pos);
		messagePart = line.substr(pos + 2);
	}
	else
		commandPart = line;

	std::istringstream iss(commandPart);
	std::string word;

	if (iss >> word)
		cmd.name = word;
	while (iss >> word)
		cmd.params.push_back(word);

	if (!messagePart.empty())
		cmd.params.push_back(messagePart);

	return (cmd);
}

void Server::handleCommand(Client& client, const Command& cmd)
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
	else
		sendError(client, "421", cmd.name + " :Unknown command");

	if (client.isRegistered() && !client.hasWelcomed())
	{
		sendWelcome(client);
		client.setWelcomed(true);
	}
}

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

void Server::sendMode(Client& client, std::string channelName, std::string&  modes, std::string&  params) {
	std::string output 		= ":ircserv 324 " + client.getNick() + " " + channelName + " " + modes;
	if(!params.empty())
		output += " " + params;
	output += "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}

static std::string toString(size_t n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

void Server::handleMode(Client& client, const Command& cmd) {

	if(!client.isRegistered()) {
		sendError(client, "451", " :You have not registered");
		return;
	}
	if(cmd.params.size() < 1) {
		sendError(client, "461", "MODE :Not enough parameters");
		return;
	}
	std::string channelName = cmd.params[0];
	Channel *channel = findChannelByName(channelName);
	if(!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if(!channel->hasMember(client.getFd())) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	// MODE #channel
	if(cmd.params.size() == 1) {
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
		if(channel->hasLimit()) {
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
	if(modeStr[0] != '+' && modeStr[0] != '-') {
		sendError(client, "501", client.getNick() + " :Unknown MODE flag");
		return;
	}
	if(!channel->isModerator(client.getFd()))  {
				sendError(client, "482", channelName + " :You're not channel operator");
				return;
	}
	char sign = modeStr[0];
	int paramsCnt = cmd.params.size();
	int paramsIdx = 2;
	for(size_t i = 1; i < modeStr.length(); i++) {
		if (modeStr[i] == '+' || modeStr[i] == '-' ) {
			sign = modeStr[i];
			continue;
		}
		else if	(modeStr[i] == 't') {
			
			bool oldValue = channel->isTopicRestricted(); //+t -
			if(sign == '+')
				channel->setTopicRestricted(true);
			else if(sign == '-')
				channel->setTopicRestricted(false);
			if(oldValue != channel->isTopicRestricted()) {
					std::string msg = 	":" + client.getNick() + "!" + client.getUser()
										+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "t" + "\r\n";
					broadcastToChannel(*channel, msg, -1);
			}
		}
		else if (modeStr[i] == 'i') {
			bool oldValue = channel->isInviteOnly();
			if(sign == '+')
				channel->setInviteOnly(true);
			else if(sign == '-')
				channel->setInviteOnly(false);
			if(oldValue != channel->isInviteOnly()) {
					std::string msg = 	":" + client.getNick() + "!" + client.getUser()
										+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "i" + "\r\n";
					broadcastToChannel(*channel, msg, -1);
			}
		}
		else if (modeStr[i] == 'k') {
			bool oldHasKey = channel->hasKey();
			std::string oldKey;
			if (oldHasKey)
				oldKey = channel->getKEy();

			std::string newKey;

			if (sign == '+') {
				if (paramsIdx >= paramsCnt) {
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
			if (sign == '+') {
				if (!oldHasKey)
					changed = true;
				else {
					if (oldKey != newKey)
						changed = true;
				}
			}
			else if (sign == '-') {
				if (oldHasKey)
					changed = true;
			}

			if (changed) {
				std::string msg = ":" + client.getNick() + "!" + client.getUser()
					+ "@localhost MODE " + channelName + " " + std::string(1, sign) + "k\r\n";
				broadcastToChannel(*channel, msg, -1);
			}
		}

		else if (modeStr[i] == 'l') {

		}
		else if (modeStr[i] == 'o') {

		}
		else {
			std::string x(1, modeStr[i]);
			sendError(client, "472", client.getNick() + " " + x + " :is unknown mode char to me");
			return;
		}
	}

}

void Server::handleTopic(Client& client, const Command& cmd) {
	if(!client.isRegistered()) {
		sendError(client, "451", " :You have not registered");
		return;
	}
	if(cmd.params.size() < 1) {
		sendError(client, "461", "TOPIC :Not enough parameters");
		return;
	}
	std::string const& channelName = cmd.params[0];
	Channel *channel = findChannelByName(channelName);
	if(!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if(!channel->hasMember(client.getFd())) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	if(cmd.params.size() == 1) {
		if(channel->topic() == "")
			sendTopic(client, *channel, "331", " :No topic is set");
		else
			sendTopic(client, *channel, "332", " :" + channel->topic());
		return;
	}
	if( channel->isTopicRestricted() && !channel->isModerator(client.getFd())) {
		sendError(client, "482", channelName + " :You're not channel operator");
		return;
	}
	std::string const& newTopic = cmd.params[1];
	channel->setTopic(newTopic);

	std::string msg = ":" + client.getNick() + "!" + client.getUser()
					+ "@localhost TOPIC " + channel->name()
					+ " :" + newTopic + "\r\n";
	broadcastToChannel(*channel, msg, -1);
}


//KICK #a <nickB> :bye
void	Server::handleKick(Client &client, const Command& cmd) {
	

	if(!client.isRegistered()) {
		sendError(client, "451", " :You have not registered");
		return;
	}
	if(cmd.params.size() < 2) {
		sendError(client, "461", "KICK :Not enough parameters");
		return;
	}
	std::string const& channelName = cmd.params[0];
	std::string const& targetNick = cmd.params[1];
	Channel *channel = findChannelByName(channelName);
	if(!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if(!channel->hasMember(client.getFd())) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	if(!channel->isModerator(client.getFd())) {
		sendError(client, "482", channelName + " :You're not channel operator");
		return;
	}
	Client *clientTarget = findClientByNick(targetNick);
	if(!clientTarget) {
		sendError(client, "401", targetNick + " :No such nick");
		return;
	}
	if(!channel->hasMember(clientTarget->getFd())) {
		sendError(client, "441", targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}
	std::string reason = " :Kicked!";
	if(cmd.params.size() > 2)
		reason = " :" + cmd.params[2];
	std::string kickMsg 	= ":" + client.getNick() + "!" + client.getUser() + "@localhost KICK "
							+ channel->name() + " " + clientTarget->getNick() +  reason + "\r\n"; 
	broadcastToChannel(*channel, kickMsg, -1);
	channel->removeMember(clientTarget->getFd());
	if(channel->empty())
		_channels.erase(channelName);
}

// INVITE <nick> <#channel>
void Server::handleInvite(Client& client, const Command& cmd)
{
	if (!client.isRegistered()) {
		sendError(client, "451", " :You have not registered");
		return;
	}
	if (cmd.params.size() < 2) {
		sendError(client, "461", "INVITE :Not enough parameters");
		return;
	}

	std::string const& targetNick  = cmd.params[0];
	std::string const& channelName = cmd.params[1];
	Channel* channel = findChannelByName(channelName);
	if (!channel) {
		sendError(client, "403", channelName + " :No such channel");
		return;
	}
	if (!channel->hasMember(client.getFd())) {
		sendError(client, "442", channelName + " :You're not on that channel");
		return;
	}
	if (!channel->isModerator(client.getFd())) {
		sendError(client, "482", channel->name() + " :You're not channel operator");
		return;
	}

	Client* target = findClientByNick(targetNick);
	if (!target) {
		sendError(client, "401", targetNick + " :No such nick");
		return;
	}

	if (channel->hasMember(target->getFd())) {
		sendError(client, "443", targetNick + " " + channelName + " :is already on channel");
		return;
	}
	if (channel->isInvited(target->getFd())) {
		sendError(client, "443", targetNick + " " + channelName + " :is already invited");
		return;
	}
	channel->invite(target->getFd());
	sendInvit(client, *target, *channel);
	sendReply(client, *target, *channel, "341");
}


void Server::handlePass(Client& client, const Command& cmd)
{
	if (client.passAccepted())
	{
		sendError(client, "462", " :You may not reregister");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "461", "PASS :Not enough parameters");
		return;
	}
	if (cmd.params[0] != _password)
	{
		sendError(client, "464", " :Password incorrect");
		return;
	}
	client.setPassAccepted(true);
}

void Server::handleNick(Client& client, const Command& cmd)
{
	if (!client.passAccepted())
	{
		sendError(client, "451", " :You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "431", " *:No nickname given");
		return;
	}
	std::string nickname = cmd.params[0];
	if (nicknameExists(nickname))
	{
		sendError(client, "433", "* " + nickname + " :Nickname is already use");
		return;
	}
	client.setNick(nickname);
}

void Server::handleUser(Client& client, const Command& cmd)
{
	if (!client.passAccepted())
	{
		sendError(client, "451", " :You have not registered");
		return;
	}
	if (cmd.params.size() < 4)
	{
		sendError(client, "461", "User :Not enough parameters");
		return;
	}
	client.setUser(cmd.params[0]);
}
// JOIN #chan
void Server::handleJoin(Client& client, const Command& cmd)
{
	 if (!client.isRegistered())
	{
		sendError(client, "451", " :You have not registered");
		return;
	}
	if (cmd.params.size() < 1)
	{
		sendError(client, "461", "JOIN :Not enough parameters");
		return;
	}
	std::string channelName = cmd.params[0];
	if (channelName[0] != '#')
	{
		sendError(client, "476", channelName + " :Bad Channel Mask");
		return;
	}
	if (_channels.find(channelName) == _channels.end())
		_channels.insert(std::make_pair(channelName, Channel(channelName)));
	Channel& channel = _channels[channelName];
	// JOIN #chan 

	if(channel.isInviteOnly()) //if #chan +i
	{
		if(!channel.isInvited(client.getFd())) {
			sendError(client, "473", client.getNick() + channelName + " :Cannot join channel (+i)");
			return;
		}
	}
	
	channel.addMember(client.getFd());
	if (channel.memberCount() == 1)
		channel.addModerator(client.getFd());
	
	std::string msg = ":" + client.getNick() + "!" + client.getUser() + "@localhost JOIN " + channelName + "\r\n";
	broadcastToChannel(channel, msg, -1);
	sendNames(client, channel);
}
// PRIVMSG

bool Server::nicknameExists(const std::string& nick)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNick() == nick)
			return (true);
	}
	return (false);
}

// on dois tro

//:ircserv 332 <nick> <#chan> :<topic>
void Server::sendTopic(Client& client, Channel& channel, std::string const& code, const std::string& message) {
	std::string output 		= ":ircserv " + code + " " + client.getNick() + " " 
							+ channel.name() + message + "\r\n";
	send(client.getFd(), output.c_str(), output.size(), 0);
}


void Server::sendReply(Client& client, Client& target, Channel& channel, std::string const& code) {
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
	std::string output = ":ircserv " + code + " * " + message + "\r\n";
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

void Server::broadcastToChannel(Channel& channel, const std::string& msg, int exceptFd)
{
    const std::set<int>& mbrs = channel.members();
    for (std::set<int>::const_iterator it = mbrs.begin(); it != mbrs.end(); ++it)
    {
        int fd = *it;
        if (fd == exceptFd) continue;
        send(fd, msg.c_str(), msg.size(), 0);
    }
}



