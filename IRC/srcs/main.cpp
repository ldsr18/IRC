/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecarro <jdecarro@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 08:43:54 by jdecarro          #+#    #+#             */
/*   Updated: 2026/02/06 08:43:54 by jdecarro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

void print_banner() {
	std::cout << CYAN << BOLD << std::endl;
	std::cout << "  ___ _____     ___ ____   ____ " << std::endl;
	std::cout << " | __|_   _|   |_ _|  _ \\ / ___|" << std::endl;
	std::cout << " | _|  | |      | || |_) | |    " << std::endl;
	std::cout << " |_|   |_|     |___|  _ <| |___ " << std::endl;
	std::cout << "                   |_| \\_\\\\____|" << std::endl;

	std::cout << GREY << "       By: jdecarro and mbendidi" << RESET << std::endl;
	std::cout << MAGENTA << BOLD << "\n           -- FT_IRC SERVER --       " << RESET << std::endl;
	std::cout << std::endl;
}


void print_usage_guide() {
	std::cout << YELLOW << BOLD
				<< "--- Supported IRC Commands (Mandatory Scope) ---"
				<< RESET << std::endl;

	std::cout << CYAN << BOLD << "  Registration:" << RESET << std::endl;
	std::cout << "  • PASS <password>" << std::endl;
	std::cout << "  • NICK <nickname>" << std::endl;
	std::cout << "  • USER <user> <0> <*> :<realname>" << std::endl;

	std::cout << std::endl;

	std::cout << CYAN << BOLD << "  Messaging & Channels:" << RESET << std::endl;
	std::cout << "  • JOIN <#channel>" << std::endl;
	std::cout << "  • PRIVMSG <target> :<message>" << std::endl;

	std::cout << std::endl;

	std::cout << MAGENTA << BOLD << "  Channel Control:" << RESET << std::endl;
	std::cout << "  • KICK <#chan> <nick> [:reason]" << std::endl;
	std::cout << "  • INVITE <nick> <#chan>" << std::endl;
	std::cout << "  • TOPIC <#chan> [:new_topic]" << std::endl;
	std::cout << "  • MODE <#chan> <+/-itkol> [params]" << std::endl;
	std::cout << "      - i (invite-only)" << std::endl;
	std::cout << "      - t (topic restricted)" << std::endl;
	std::cout << "      - k (channel key)" << std::endl;
	std::cout << "      - l (user limit)" << std::endl;
	std::cout << "      - o (operator)" << std::endl;
	std::cout << "  • PART <#chan>" << std::endl;
	std::cout << "  • QUIT" << std::endl;

	std::cout << YELLOW
				<< "--------------------------------------------------------"
				<< RESET << std::endl << std::endl;
}

bool server_running = true;

void signalHandler(int signum) {
	(void)signum;
	server_running = false;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << BOLD << "Usage: " << RESET << "./ircserv <port> <password>" << std::endl;
        return (1);
    }

    print_banner();
    print_usage_guide();

	signal(SIGINT, signalHandler);
    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    
    std::cout << BOLD << "Status: " << GREEN << "Starting server on port " << port << "..." << RESET << std::endl;

    try
	{
        Server serv(port, password);
        serv.run();
    }
	catch (const std::exception& e)
	{
        std::cerr << "\033[31mFatal error: " << e.what() << RESET << std::endl;
        return (1);
    }

    return (0);
}