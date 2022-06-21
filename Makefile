
.PHONEY : all clean fclean re bonus run

## variables
NAME    = webserv
CC		= clang++
FLAGS   = -w -std=c++98 -D DEBUG -D CONSOLE_ON
#-Wall -Werror -Wextra
INCLUDES= includes 

UTILS	= helpers.cpp
CONFIG	= Configuration.cpp Parser.cpp Tokenizer.cpp Directive.cpp Context.cpp
SERVER	= Cluster.cpp VServer.cpp Socket.cpp Request.cpp Response.cpp

KQUEUE_FLAGS	= ./src/EventMonitors/KQueue.cpp -D WITH_KQUEUE
EPOLL_FLAGS	= ./src/EventMonitors/EPoll.cpp -D WITH_EPOLL
POLL_FLAGS	= ./src/EventMonitors/Poll.cpp -D WITH_POLL

SRCS    = 	./src/webserv.cpp \
			$(UTILS:%.cpp=./src/utils/%.cpp)\
			$(CONFIG:%.cpp=./src/Configuration/%.cpp)\
			$(SERVER:%.cpp=./src/Networking/%.cpp)\
			$(EVENT-MONITORS:%.cpp=./src/EventMonitors/%.cpp)

HEADERS =	$(CONFIG:%.cpp=./src/Configuration/%.hpp)\
			$(SERVER:%.cpp=./src/Networking/%.hpp)\
			./src/utils/Console.hpp\
			./src/utils/Logger.hpp

CONFIG_FILE = ./other/sample.conf
## rules
$(NAME): $(SRCS) $(HEADERS)
		@$(CC) $(FLAGS) $(SRCS) $(POLL_FLAGS)\
		-I $(INCLUDES) -o $(NAME)

all: $(NAME)

with-kqueue:
ifneq (,$(findstring $(shell uname -s), Darwin BSD))
	@echo "Building with kqueue..."
	@$(CC) $(FLAGS) $(SRCS) $(KQUEUE_FLAGS) \
	-I $(INCLUDES) -o $(NAME)
else
	@echo "kqueue is not available on this platform."
	@echo -n "kqueue is only present in BSD (FreeBSD / OpenBSD)"
	@echo " and Darwin (Mac OS X / iOS) kernels."
endif

# with-epoll:
# 	@echo "Building with epoll..."
# 	@$(CC) $(FLAGS) $(SRCS) $(EPOLL_FLAGS) \
# 	-I $(INCLUDES) -o $(NAME)

clean:
	@rm -rf *.o .objects/*.o

fclean: clean
	@rm -rf $(NAME)

re:
	@echo "Re-building..."
	@make fclean
	@make

run: $(NAME)
	@./$(NAME) $(CONFIG_FILE)

TRACE=network
strace: $(NAME)
	@mkdir -p .strace/$(shell date +"%d-%m-%y")
	@strace -o .strace/$(shell date +"%d-%m-%y")/$(shell date +"%H:%M:%S").log \
	-f -e trace=${TRACE} ./$(NAME) $(CONFIG_FILE) \

setup-dirs:
#Lazy shells forced me to be explicit at dirs' names
	@mkdir -p /tmp/ws/default /tmp/ws/a /tmp/ws/b /tmp/ws/c 
	@mkdir -p /tmp/ws/a/inside
	@echo "<html><body><h1>Hello From /tmp/ws/default</h1></body></html>" > /tmp/ws/default/index.html
	@echo "<html><body><h1>Hello From /tmp/ws/a</h1></body></html>" > /tmp/ws/a/index.html
	@echo "<html><body><h1>Hello From /tmp/ws/b</h1></body></html>" > /tmp/ws/b/index.html
	@echo "<html><body><h1>Hello From /tmp/ws/c</h1></body></html>" > /tmp/ws/c/index.html
	@echo "<html><body><h1>Hello From /tmp/ws/a/inside</h1></body></html>" > /tmp/ws/a/inside/index.html
	@echo "<html><body><h1>Hello From /tmp/ws/a/inside/file.txt</h1></body></html>" > /tmp/ws/a/inside/file.txt
	@chmod -R 777 /tmp/ws