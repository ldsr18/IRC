#ifndef MYBOT_HPP
#define MYBOT_HPP


#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cstring>
#include <stdexcept>


// Couleurs ANSI
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define GREY    "\033[90m"

class Mybot {
	private:
		std::string	_addr_ip;
		std::string	_pass;
		int			_port;
		int 		_sock_fd;
		std::string	_storage;
		bool _alive;


	public:
		Mybot();
		Mybot(std::string	addr_ip, int port, std::string pass);
		Mybot(Mybot const& other);
		~Mybot();

		Mybot& operator=(Mybot const& other);

		void sendHelp(std::string const& user);
		void sendMessage(std::string target, std::string msg);
		void recv_Priv_Msg(std::string message);
		void interaction();
		void receive_msg(char *buff);
		void init_bot();
		void run_bot();
};

#endif