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
