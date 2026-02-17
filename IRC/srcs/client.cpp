/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 11:00:02 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/09 11:00:02 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

Client::Client() : _fd(-1), _passAccepted(false), _hasNick(false), _hasUser(false), _welcomed(false)
{
}

Client::Client(int fd) : _fd(fd), _passAccepted(false), _hasNick(false), _hasUser(false), _welcomed(false)
{
}

Client::Client(const Client& copy)
{
	*this = copy;
}

Client& Client::operator=(const Client& src)
{
	if (this != &src)
	{
		_fd = src._fd;
		_buffer = src._buffer;
		_passAccepted = src._passAccepted;
		_hasNick = src._hasNick;
		_hasUser = src._hasUser;
		_welcomed = src._welcomed;
		_nickname = src._nickname;
		_username = src._username;
	}
	return (*this);
}

Client::~Client()
{
}

bool Client::passAccepted() const
{
	return (_passAccepted);
}

bool Client::hasNick() const
{
	return (_hasNick);
}

bool Client::hasUser() const
{
	return (_hasUser);
}

bool Client::isRegistered() const
{
	return (_passAccepted && _hasNick && _hasUser);
}

bool Client::hasWelcomed() const
{
	return (_welcomed);
}

int Client::getFd() const
{
	return (_fd);
}

std::string& Client::getBuffer()
{
	return (_buffer);
}

const std::string& Client::getNick() const
{
	return (_nickname);
}

const std::string& Client::getUser() const
{
	return (_username);
}

void Client::setWelcomed(bool value)
{
	_welcomed = value;
}

void Client::setPassAccepted(bool value)
{
	_passAccepted = value;
}

void Client::setNick(const std::string& nick)
{
	_nickname = nick;
	_hasNick = true;
}

void Client::setUser(const std::string& user)
{
	_username = user;
	_hasUser = true;
}
