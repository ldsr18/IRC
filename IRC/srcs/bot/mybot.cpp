#include "mybot.hpp"

Mybot::Mybot() 
	: 	_addr_ip("127.0.0.1"),
		_pass("pass"),
		_port(6667),
		_sock_fd(-1),
		_storage(""),
		_alive(true)
{}

Mybot::Mybot(std::string addr_ip, int port, std::string pass)
	:	_addr_ip(addr_ip),
		_pass(pass),
		_port(port),
		_sock_fd(-1),
		_storage(""),
		_alive(true)
{}

Mybot::Mybot(Mybot const& other) 
	:	_addr_ip(other._addr_ip),
		_pass(other._pass),
		_port(other._port),
		_sock_fd(-1),
		_storage(other._storage),
		_alive(true)
{}

Mybot::~Mybot() {
	if (_sock_fd != -1) {
		close(_sock_fd);
	}
	_sock_fd = -1;
}


Mybot& Mybot::operator=(Mybot const& other) {
	if(this != &other) {
		_addr_ip = other._addr_ip;
		_pass = other._pass;
		_port = other._port;
		_sock_fd = -1;
		_storage = other._storage;
		_alive = other._alive;
	}
	return *this;
}

void Mybot::sendMessage(std::string target, std::string msg) {
    std::string fullMsg = "PRIVMSG " + target + " :" + msg + "\r\n";
    send(_sock_fd, fullMsg.c_str(), fullMsg.size(), 0);
}

void Mybot::sendHelp(std::string const& user) {
	sendMessage(user, "================ MYBOT HELP ================");
	sendMessage(user, "--- Bot Commands ---");
	sendMessage(user, "!help   : Show this usage guide");
	sendMessage(user, "!hello  : Say hi to MyBot");
	sendMessage(user, "!joke   : Get a random developer joke");
	sendMessage(user, " ");

	sendMessage(user, "--- Server Commands (Registration) ---");
	sendMessage(user, "• PASS <password>");
	sendMessage(user, "• NICK <nickname>");
	sendMessage(user, "• USER <user> <0> <*> :<realname>");
	sendMessage(user, " ");

	sendMessage(user, "--- Messaging & Channels ---");
	sendMessage(user, "• JOIN <#channel>");
	sendMessage(user, "• PRIVMSG <target> :<message>");
	sendMessage(user, "• NOTICE <target> :<message>");
	sendMessage(user, "• PART <#channel>");
	sendMessage(user, "• QUIT");
	sendMessage(user, " ");

	sendMessage(user, "--- Channel Control (Operators) ---");
	sendMessage(user, "• KICK <#chan> <nick> [:reason]");
	sendMessage(user, "• INVITE <nick> <#chan>");
	sendMessage(user, "• TOPIC <#chan> [:new_topic]");
	sendMessage(user, "• MODE <#chan> <+/-itkol> [params]");
	sendMessage(user, "    - i (invite-only) | t (topic restricted)");
	sendMessage(user, "    - k (channel key) | l (user limit) | o (operator)");
	sendMessage(user, "============================================");
}

void Mybot::recv_Priv_Msg(std::string message) {
	size_t pos_u = message.find('!');
	std::string user = "";
	if (pos_u != std::string::npos && message[0] == ':') {
		user = message.substr(1, pos_u - 1);
	}

	size_t pos_m = message.find(" :");
	if (pos_m != std::string::npos && !user.empty()) {
		std::string content = message.substr(pos_m + 2);
		
		if (content.find("!hello") == 0) {
			sendMessage(user, "Hi! I'm your MyBot for this ircserver.");
		}
		else if (content.find("!help") == 0)
			sendHelp(user);
		else if (content.find("!joke") == 0) {
			std::string jokes = "I'm not joking ...... I'm a bot! 🤖";
			sendMessage(user, jokes);
		}
		else if (content.find("!quit") == 0) {
			if (user == "admin") {
				sendMessage(user, "Bye! Shutdown in progress...");
				_alive = false;
			}
			else
				sendMessage(user, "You are not my admin");
		}
		else if (!content.empty()){
			if(content[0] == '!')
    		sendMessage(user, "Unknown command. Type !help to see what I can do!");
		}
	}
}

void Mybot::interaction() {
	size_t pos;
	while ((pos = _storage.find("\r\n")) != std::string::npos)
	{
		std::cout << "MyBot waiting ..." << std::endl;
		std::string message = _storage.substr(0, pos);
		// message = :nik!n@localhost PRIVMSG MyBot :hello
		_storage.erase(0, pos + 2);
		// _storage  = "le reste ... \r\n ..."

		if (message.find("PRIVMSG") != std::string::npos)
			recv_Priv_Msg(message);
		else
			std::cout << "debug message : " << message << std::endl;

	}
}

void Mybot::receive_msg(char *buff) {
	while(_alive) {
        int valrecv = recv(_sock_fd, (void *)buff, 511, 0); // !hello ==> !hel \n\r ==> lo\r\n
        if(valrecv <= 0) break;
        buff[valrecv] = '\0';
        _storage += buff;

        std::cout << "Server said :\n" << _storage << std::endl;
        interaction();
	}
}

void Mybot::init_bot() {
	_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_sock_fd < 0)
		throw std::runtime_error("Socket failed!");

	sockaddr_in bot_addr;
    std::memset(&bot_addr, 0, sizeof(bot_addr));
    bot_addr.sin_family = AF_INET;
    bot_addr.sin_port = htons(_port);

	int ip_check = inet_pton(AF_INET, _addr_ip.c_str(), &bot_addr.sin_addr);
	if(ip_check <= 0) {
		close(_sock_fd);
		_sock_fd = -1;
		throw std::runtime_error("Error :Addresse IP invalid!");
    }
	int status = connect(_sock_fd, (sockaddr *)&bot_addr, sizeof(bot_addr));
    if(status < 0) {
		close(_sock_fd);
		_sock_fd = -1;
		throw std::runtime_error("Error connect to addr!");
    }
}

void Mybot::run_bot() {
	std::cout << "running...." << std::endl;

	init_bot();

	std::cout << "MyBot listenning" << std::endl;

    std::string auth = "PASS " + _pass + "\r\nNICK MyBot\r\nUSER MyBot * 0 :MyBot\r\n";
    send(_sock_fd, auth.c_str(), auth.size(), 0);

    char buff[512];
    std::memset(&buff, 0, sizeof(buff));
	receive_msg(buff);
	std::cout << "close fd" << std::endl;
	if (_sock_fd != -1) {
		close(_sock_fd);
	}
	_sock_fd = -1;
}
