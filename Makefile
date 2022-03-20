

.PHONEY: all clean fclean re bonus run

NAME    = webserv
CC		= clang++ --std=c++98
FLAGS   = #-Wall -Werror -Wextra
INCLUDE = -I includes -I ../includes

PARSER  = 
REQ		= 
RES		= 


SRCS    = 	./src/webserv.cpp\
			$(PARSER:%.cpp=./src/parser/%.cpp)

$(NAME): $(SRCS)
	@$(CC) $(FLAGS) $(INCLUDE) $(SRCS) -o $(NAME)

all: $(NAME)

clean:
	@rm -rf *.o

fclean: clean
	@rm -rf $(NAME)

re: fclean all

run: fclean all
	@./$(NAME) ./webserv.conf
