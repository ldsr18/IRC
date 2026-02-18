NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincs

SRCS =	srcs/channel.cpp \
		srcs/client.cpp \
		srcs/main.cpp \
		srcs/commands/invite.cpp \
		srcs/commands/join.cpp \
		srcs/commands/kick.cpp \
		srcs/commands/mode.cpp \
		srcs/commands/nick.cpp \
		srcs/commands/part.cpp \
		srcs/commands/pass.cpp \
		srcs/commands/ping.cpp \
		srcs/commands/privmsg.cpp\
		srcs/commands/quit.cpp\
		srcs/commands/topic.cpp \
		srcs/commands/user.cpp \
		srcs/server/commandHandler.cpp \
		srcs/server/server.cpp \
		srcs/server/serverFind.cpp \
		srcs/server/serverReceive.cpp \
		srcs/server/serverSocket.cpp \
		srcs/utils/broadcast.cpp \
		srcs/utils/replies.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
