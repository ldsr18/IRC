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

			bool _passAccepted;
			bool _hasNick;
			bool _hasUser;
			bool _welcomed;
			
			std::string _nickname;
			std::string _username;

	public:
			Client();
			Client(int fd);
			Client(const Client& copy);
			Client& operator=(const Client& src);
			~Client();

			//getter d'état
			bool passAccepted() const;
			bool hasNick() const;
			bool hasUser() const;
			bool isRegistered() const;
			bool hasWelcomed() const;

			//getter
			int getFd() const;
			std::string& getBuffer();
			const std::string& getNick() const;
			const std::string& getUser() const;
			void setWelcomed(bool value);

			//setter
			void setPassAccepted(bool value);
			void setNick(const std::string& nick);
			void setUser(const std::string& user);
};

#endif