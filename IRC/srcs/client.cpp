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

#include "../incs/client.hpp"

Client::Client(int fd) : _fd(fd)
{
}

int Client::getFd() const
{
	return (_fd);
}

std::string& Client::getBuffer()
{
	return (_buffer);
}