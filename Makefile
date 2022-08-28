
.PHONEY : all clean fclean re run strace setup-dirs with-kqueue

NAME    = webserv
CC		= g++ -std=c++98
FLAGS   = -Wall -Werror -Wextra

INCLUDES= includes 

UTILS	= helpers.cpp mimeTypes.cpp
CONFIG	= Configuration.cpp Parser.cpp Tokenizer.cpp Directive.cpp Context.cpp
SERVER	= Cluster.cpp VServer.cpp Socket.cpp Request.cpp Response.cpp
CGI		= CGI.cpp CGI_utils.cpp

KQUEUE_FLAGS	= ./src/EventMonitors/KQueue.cpp -D WITH_KQUEUE
EPOLL_FLAGS	= ./src/EventMonitors/EPoll.cpp -D WITH_EPOLL
POLL_FLAGS	= ./src/EventMonitors/Poll.cpp -D WITH_POLL

SRCS    = 	./src/webserv.cpp \
			$(UTILS:%.cpp=./src/utils/%.cpp)\
			$(CONFIG:%.cpp=./src/Configuration/%.cpp)\
			$(SERVER:%.cpp=./src/Networking/%.cpp)\
			$(EVENT-MONITORS:%.cpp=./src/EventMonitors/%.cpp)\
			$(CGI:%.cpp=./src/CGI/%.cpp)

HEADERS =	$(CONFIG:%.cpp=./src/Configuration/%.hpp)\
			$(SERVER:%.cpp=./src/Networking/%.hpp)\
			./src/utils/Console.hpp\
			./src/utils/Logger.hpp

CONFIG_FILE			= ./conf/default.conf

#Test dirs
TEST_DIR_ROOT		= /tmp/ws
TEST_DIR_DEFAULT	= /tmp/ws/default
TEST_DIR_A 			= /tmp/ws/a
TEST_DIR_INSIDE_A 	= /tmp/ws/a/inside
TEST_DIR_B			= /tmp/ws/b
TEST_DIR_C			= /tmp/ws/c
TEST_DIR_UPLOAD		= /tmp/ws/upload
TEST_DIR_CGI		= /tmp/ws/cgi

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

re: fclean
	@echo "Re-building..."
	@make

run: $(NAME)
	@./$(NAME) $(CONFIG_FILE)


TRACE=network
strace: $(NAME)
	@mkdir -p .strace/$(shell date +"%d-%m-%y")
	@strace -o .strace/$(shell date +"%d-%m-%y")/$(shell date +"%H:%M:%S").log \
	-f -e trace=${TRACE} ./$(NAME) $(CONFIG_FILE) \

setup: setup-dirs all

setup-dirs:
#Lazy shells forced me to be explicit at dirs' names
	@mkdir -p $(TEST_DIR_DEFAULT) $(TEST_DIR_A) $(TEST_DIR_B) $(TEST_DIR_C)\
		$(TEST_DIR_INSIDE_A) $(TEST_DIR_UPLOAD) $(TEST_DIR_CGI)
	@tar -xzf ./other/static-website.tar.gz -C $(TEST_DIR_DEFAULT) --strip-components 1
	@cp -r ./other/hello.* $(TEST_DIR_CGI)
	@cp -r ./other/form.html $(TEST_DIR_C)
	@echo "<html><body><h1>Hello From $(TEST_DIR_A)</h1></body></html>" > $(TEST_DIR_A)/index.html
# @echo "<html><body><h1>Hello From $(TEST_DIR_B)</h1></body></html>" > $(TEST_DIR_B)/index.html
	@echo "<html><body><h1>Hello From $(TEST_DIR_C)</h1></body></html>" > $(TEST_DIR_C)/index.html
	@echo "<html><body><h1>Hello From $(TEST_DIR_INSIDE_A)</h1></body></html>" > $(TEST_DIR_INSIDE_A)/index.html
	@echo "<html><body><h1>Hello From $(TEST_DIR_INSIDE_A)/file.txt</h1></body></html>" > $(TEST_DIR_INSIDE_A)file.txt
# custom error pages
	@echo "<html><head><title>404 Not Found</title></head><body style=\"background-color:#c9d1c9;color:#000000;text-align:center;\">\
		<h1 style=\"padding:12px;border-bottom:1px solid gray;\">404 - Page not found</h1></body></html>" > $(TEST_DIR_DEFAULT)/404.html
	@echo "<html><head><title>403 Forbidden</title></head><body style=\"background-color:#c9d1c9;color:#000000;text-align:center;\">\
		<h1 style=\"padding:12px;border-bottom:1px solid gray;\">403 - Forbidden</h1></body></html>" > $(TEST_DIR_DEFAULT)/403.html

	@chmod -R 777 $(TEST_DIR_ROOT)
