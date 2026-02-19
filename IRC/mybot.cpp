#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <ctime>
#include <sstream>
#include <stdlib.h>
#include <cstring>




int main() {

	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in bot_addr;
	std::memset(&bot_addr, 0, sizeof(bot_addr));
	bot_addr.sin_family = AF_INET;
	bot_addr.sin_addr.s_addr = "127.0.0.1";
	bot_addr.sin_port = 6667;

	int status = connect(sock_fd, (sockaddr *)&bot_addr, sizeof(bot_addr));
	if(status) {
		std::cerr << "Error connect to addr" << std::endl;
		close(sock_fd);
		return 1;
	}
	while(true) {

	}
	close(sock_fd);
	return 0;
}
