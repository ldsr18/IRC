#include "mybot.hpp"

int main(int ac, char **av) {
	// mybot 127.0.0.1 6667
	if(ac != 4) {
		std::cerr << "USE : ./mybot <addr_ip> <port> <password>" << std::endl;
		return 1;
	}
	std::string addr_ip(av[1]);
	int port = atoi(av[2]);
	std::string pass(av[3]);

	Mybot mybot(addr_ip, port, pass);
	try {
		mybot.run_bot();
	}
	catch (std::exception &e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		return 1;
	}

	std::cout << BOLD << "bey...:)" << RESET << std::endl;
	return 0;
}
