

.PHONEY : all clean fclean re bonus run

NAME    = webserv
CC		= clang++ --std=c++98
FLAGS   = #-Wall -Werror -Wextra
INCLUDES= -I includes 

UTILS	= helpers.cpp
CONFIG	= Configuration.cpp Parser.cpp Tokenizer.cpp Directive.cpp
REQ		= 
RES		= 
SERVER	= Server.cpp Socket.cpp


SRCS    = 	./src/webserv.cpp \
			$(UTILS:%.cpp=./src/utils/%.cpp)\
			$(CONFIG:%.cpp=./src/Configuration/%.cpp)\
			$(SERVER:%.cpp=./src/Networking/%.cpp)


$(NAME): $(SRCS)
	@$(CC) $(FLAGS) $(INCLUDES) $(SRCS) -o $(NAME)

all: $(NAME)

clean:
	@rm -rf *.o

fclean: clean
	@rm -rf $(NAME)

re: fclean all

run: fclean all
	@./$(NAME) ./other/sample.conf
