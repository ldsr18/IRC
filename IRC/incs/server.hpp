/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 08:45:12 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/06 08:45:12 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "client.hpp"
#include "command.hpp"
#include "channel.hpp"

#include <iostream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <map>
#include <sstream>

#include <unistd.h> //close
#include <fcntl.h>
#include <sys/socket.h> //socket, bind, listen...
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //htons
#include <poll.h> //poll, pollfd


// Couleurs ANSI
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define GREY    "\033[90m"

class Server
{
	private:
			int _port;
			int _serverFd; //fd du serveur = le socket qui écoute
			std::string _password;
			std::map<std::string, Channel> _channels;

			std::vector<struct pollfd> _fds;
			std::map<int, Client> _clients;

			//réseau
			void setupSocket(); //crée et prépare lee serveur
			void acceptClient();
			bool receiveFromClient(int clientFd);
			void removeFd(int fd);

			//parsing command
			Command parseCommand(const std::string& line);
			void handleCommand(Client& client, const Command& cmd);

			//gestion authentification
			void handlePass(Client& client, const Command& cmd);
			void handleNick(Client& client, const Command& cmd);
			void handleUser(Client& client, const Command& cmd);
			void handleJoin(Client& client, const Command& cmd);
			void handlePrivMSG(Client& client, const Command& cmd);
			bool nicknameExists(const std::string& nick);

			//channel
			Channel* findChannelByName(const std::string& nick);
			Client*	findClientByNick(const std::string& nick);
			void	handlePart(Client& client, const Command& cmd);
			void	handlePing(Client& client, const Command& cmd);
			void	handleMode(Client& client, const Command& cmd);
			void 	handleTopic(Client& client, const Command& cmd);
			void	handleKick(Client &client, const Command& cmd);
			void 	handleInvite(Client& client, const Command& cmd);

			//réponses
			void sendError(Client& client, const std::string& code, const std::string& message);
			void sendWelcome(Client& client);
			void sendNames(Client& client, Channel& channel);
			void sendTopic(Client& client, Channel& channel, std::string const& code, const std::string& message);
			void sendInvit(Client& client, Client& target, Channel& channel);
			void sendReply(Client& client, Client& target, Channel& channel, std::string const& code);
			void sendMode(Client& client, std::string channelName, std::string&  modes, std::string&  params);
			void broadcastToChannel(Channel& channel, const std::string& msg, int exceptFd);

			Server(const Server& other);
			Server& operator=(const Server& other);

	public:
			Server(int port, const std::string& password);
			~Server();

			void run();
};

#endif