
.PHONEY : all clean fclean re bonus run dirs

## variables
NAME    = webserv
CC		= clang++
FLAGS   = --std=c++98 #-Wall -Werror -Wextra
INCLUDES= includes 
OBJDIR  = .objects

UTILS	= helpers.cpp
CONFIG	= Configuration.cpp Parser.cpp Tokenizer.cpp Directive.cpp
REQ		= 
RES		= 
SERVER	= Server.cpp Socket.cpp

SRCS    = 	./src/webserv.cpp \
			$(UTILS:%.cpp=./src/utils/%.cpp)\
			$(CONFIG:%.cpp=./src/Configuration/%.cpp)\
			$(SERVER:%.cpp=./src/Networking/%.cpp)

HEADERS =	$(CONFIG:%.cpp=./src/Configuration/%.hpp)\
			$(SERVER:%.cpp=./src/Networking/%.hpp)\
			./src/utils/Console.hpp

## rules
$(NAME): $(SRCS) $(HEADERS)
	@$(CC) $(FLAGS) $(SRCS) -I $(INCLUDES) -o $(NAME)


all: dirs $(NAME)

clean:
	@rm -rf *.o .objects/*.o

fclean: clean
	@rm -rf $(NAME)

re: fclean all

run: fclean all
	@./$(NAME) ./other/sample.conf

# to avoid poluting the root directory with object files
dirs: .objects
