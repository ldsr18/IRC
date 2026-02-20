NAME = ircserv
NAME_BOT = mybot

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincs

SRCS = 	srcs/channel.cpp \
		srcs/client.cpp \
		srcs/main.cpp \
		srcs/commands/invite.cpp \
		srcs/commands/join.cpp \
		srcs/commands/kick.cpp \
		srcs/commands/message.cpp \
		srcs/commands/mode.cpp \
		srcs/commands/nick.cpp \
		srcs/commands/part.cpp \
		srcs/commands/pass.cpp \
		srcs/commands/ping.cpp \
		srcs/commands/quit.cpp \
		srcs/commands/topic.cpp \
		srcs/commands/user.cpp \
		srcs/server/commandHandler.cpp \
		srcs/server/server.cpp \
		srcs/server/serverFind.cpp \
		srcs/server/serverReceive.cpp \
		srcs/server/serverSocket.cpp \
		srcs/utils/broadcast.cpp \
		srcs/utils/replies.cpp

SRCS_BOT = srcs/bot/mybot.cpp srcs/bot/main_bot.cpp

OBJS = $(SRCS:.cpp=.o)
OBJS_BOT = $(SRCS_BOT:.cpp=.o)

all: $(NAME) $(NAME_BOT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

bot: $(NAME_BOT)

$(NAME_BOT): $(OBJS_BOT)
	$(CXX) $(CXXFLAGS) $(OBJS_BOT) -o $(NAME_BOT)

clean:
	rm -f $(OBJS) $(OBJS_BOT)

fclean: clean
	rm -f $(NAME) $(NAME_BOT)

re: fclean all

.PHONY: all clean fclean re bot