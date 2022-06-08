
.PHONEY : all clean fclean re bonus run

## variables
NAME    = webserv
CC		= clang++
FLAGS   = -w -std=c++98 -D DEBUG -D CONSOLE_ON
#-Wall -Werror -Wextra

INCLUDES= includes 

UTILS	= helpers.cpp
CONFIG	= Configuration.cpp Parser.cpp Tokenizer.cpp Directive.cpp 
SERVER	= Cluster.cpp VServer.cpp Socket.cpp Request.cpp Response.cpp

WITH_POLL	= ./src/EventMonitors/Poll.cpp -D WITH_POLL
WITH_KQUEUE	= ./src/EventMonitors/KQueue.cpp -D WITH_KQUEUE

SRCS    = 	./src/webserv.cpp \
			$(UTILS:%.cpp=./src/utils/%.cpp)\
			$(CONFIG:%.cpp=./src/Configuration/%.cpp)\
			$(SERVER:%.cpp=./src/Networking/%.cpp)\
			$(EVENT-MONITORS:%.cpp=./src/EventMonitors/%.cpp)

HEADERS =	$(CONFIG:%.cpp=./src/Configuration/%.hpp)\
			$(SERVER:%.cpp=./src/Networking/%.hpp)\
			./src/utils/Console.hpp\
			./src/utils/Logger.hpp

## rules
$(NAME): $(SRCS) $(HEADERS)
		@$(CC) $(FLAGS) $(SRCS) $(WITH_POLL)\
		-I $(INCLUDES) -o $(NAME)

all: $(NAME)

with-kqueue:
ifneq (,$(findstring $(shell uname -s), Darwin BSD))
	@echo "Building with kqueue..."
	@$(CC) $(FLAGS) $(SRCS) $(WITH_KQUEUE)\
	-I $(INCLUDES) -o $(NAME)
else
	@echo "kqueue is not available on this platform."
	@echo -n "kqueue is only present in BSD (FreeBSD / OpenBSD)"
	@echo " and Darwin (Mac OS X / iOS) kernels."
endif

clean:
	@rm -rf *.o .objects/*.o

fclean: clean
	@rm -rf $(NAME)

re: fclean all

run: $(NAME)
	@./$(NAME) ./other/sample.conf

TRACE=network
strace: $(NAME)
	@strace -o .strace/$(shell date +"%d-%m-%y-[%H:%M:%S]").log -f -e trace=${TRACE} ./$(NAME) ./other/sample.conf

