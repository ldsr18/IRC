/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 10:55:27 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/09 10:55:27 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

class Client
{
	private:
			int _fd;
			std::string _buffer;

	public:
			Client(int fd);

			int getFd() const;
			std::string& getBuffer();
};

#endif