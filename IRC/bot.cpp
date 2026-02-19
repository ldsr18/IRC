#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <ctime>
#include <stdlib.h>
#include <sstream>


int main () {
	std::srand(std::time(0));
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(6667);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	if(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "Connexion failed" << std::endl;
		return 1;
	}

	std::string auth = "PASS pass\r\nNICK MyBot\r\nUSER bot 0 * :I am a bot\r\n";
	send(sock_fd, auth.c_str(), auth.size(), 0);

	char buffer[512];
	while(true) {
		int valread = recv(sock_fd, &buffer, 511, 0);
		if(valread <= 0) break;
		buffer[valread] = '\0';

        std::string msg(buffer);
        std::cout << "Serveur dit : " << msg << std::endl;

		// 1. On vérifie si c'est un message privé ou de canal
		// Dans ta boucle while, après std::string msg(buffer, valread);

		if (msg.find("PRIVMSG") != std::string::npos) {
			// 1. Extraire le Pseudo (entre le premier ':' et le '!')
			size_t bang = msg.find("!");
			std::string sender = "";
			if (bang != std::string::npos && msg[0] == ':') {
				sender = msg.substr(1, bang - 1);
			}

			// 2. Extraire le texte du message (tout ce qui est après le DEUXIÈME ':')
			// On cherche le ':' qui commence après le mot PRIVMSG
			size_t trail = msg.find(" :", msg.find("PRIVMSG"));
			if (trail != std::string::npos) {
				std::string content = msg.substr(trail + 2);
				
				// Nettoyage des \r\n pour la comparaison
				if (!content.empty() && (content.find('\r') != std::string::npos || content.find('\n') != std::string::npos)) {
					content.erase(content.find_first_of("\r\n"));
				}

				// 3. Logique de réponse
				if (content == "hello" || content == "cc") {
					// On répond directement au sender (Pseudo)
					std::string response = "PRIVMSG " + sender + " :Salut " + sender + ", je suis ton bot !\r\n";
					send(sock_fd, response.c_str(), response.size(), 0);
				}

				// ... dans ta logique de contenu ...
				if (content == "!time") {
					std::time_t now = std::time(0);
					std::string dt = std::ctime(&now);
					if (!dt.empty() && dt[dt.length()-1] == '\n') dt.erase(dt.length()-1); // Nettoie le \n de ctime

					std::string response = "PRIVMSG " + sender + " :L'heure actuelle est : " + dt + "\r\n";
					send(sock_fd, response.c_str(), response.size(), 0);
				}

				if (content == "!help" || content == "!commands") {
					std::string helpMsg = "PRIVMSG " + sender + " :Je suis le Bot de 42-IRC. Voici mes commandes :\r\n";
					send(sock_fd, helpMsg.c_str(), helpMsg.size(), 0);
					
					std::string list = "PRIVMSG " + sender + " :!hello, !time, !joke, !roll, !exit (Admin)\r\n";
					send(sock_fd, list.c_str(), list.size(), 0);
				}

				if (content == "!roll") {
					int result = (std::rand() % 6) + 1;

					// ... dans le !roll ...
					std::stringstream ss;
					ss << ((std::rand() % 6) + 1);
					std::string resStr = "PRIVMSG " + sender + " : Tu as lance un " + ss.str() + " !\r\n";
					send(sock_fd, resStr.c_str(), resStr.size(), 0);
				}

				if (content == "!joke") {
					std::string jokes[] = {
						"Pourquoi les programmeurs confondent Halloween et Noel ? Parce que OCT 31 == DEC 25.",
						"Je ne bug pas. Je cree des fonctionnalites surprises.",
						"Un SQL entre dans un bar, voit deux tables et demande : 'Je peux vous JOIN ?'",
						"Il y a 10 types de personnes : ceux qui comprennent le binaire et les autres.",
						"J'ai voulu faire une blague UDP... mais je ne suis pas sur que tu l'aies recue.",
						"Pourquoi les dev aiment le mode sombre ? Parce que la lumiere attire les bugs.",
						"Mon code marche parfaitement... sauf quand je le lance.",
						"Un pointeur rentre dans un bar et demande l'adresse."
					};

					int i = std::rand() % 8;
					std::string jokeMsg = "PRIVMSG " + sender + " :" + jokes[i] + "\r\n";
					send(sock_fd, jokeMsg.c_str(), jokeMsg.size(), 0);
				}

				if (content.find("!echo ") == 0) {
					std::string toEcho = content.substr(6);
					std::string echoMsg = "PRIVMSG " + sender + " :Tu as dit : " + toEcho + "\r\n";
					send(sock_fd, echoMsg.c_str(), echoMsg.size(), 0);
				}

				if (content == "!exit" && sender == "Simo") {
					std::string quitMsg = "QUIT :Ordre de mon maître reçu.\r\n";
					send(sock_fd, quitMsg.c_str(), quitMsg.size(), 0);
					break; // Sort de la boucle while(true)
				}


			}
		}
	}
	close(sock_fd);
	return 0;
}

