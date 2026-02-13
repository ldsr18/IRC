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

class Server
{
	private:
			int _port;
			int _serverFd; //fd du serveur = le socket qui écoute
			std::string _password;

			std::vector<struct pollfd> _fds;
			std::map<int, Client> _clients;
			std::map<std::string, Channel> _channels;

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
			bool nicknameExists(const std::string& nick);

			//réponses
			void sendError(Client& client, const std::string& code, const std::string& message);
			void sendWelcome(Client& client);
			void broadcastToChannel(Channel& channel, const std::string& msg, int exceptFd)

	public:
			Server(int port, const std::string& password);
			~Server();

			void run();
};

#endif