NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes

BUILTINS = 

MAIN = main  Irc

EXECUTING = 

ERROR_MANAGEMENT =

PARSE = 

FILES = $(MAIN) \
$(BUILTINS) \
$(EXECUTING) \
$(ERROR_MANAGEMENT) \
$(PARSE)

SRC_FILES = $(addprefix src/builtins/, $(BUILTINS)) \
$(addprefix src/executing/, $(EXECUTING)) \
$(addprefix src/main/, $(MAIN)) \
$(addprefix src/error_management/, $(ERROR_MANAGEMENT)) \
$(addprefix src/parse/, $(PARSE))

OBJ_DIR = obj/

SRCS = $(addsuffix .cpp, $(SRC_FILES))
OBJS = $(addprefix $(OBJ_DIR), $(addsuffix .o, $(FILES)))

all : $(NAME)

$(NAME) : $(OBJ_DIR) $(OBJS)
	$(CC)  $(CFLAGS) -o $(NAME) $(OBJS)

obj/%.o: src/*/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re : fclean
	make

.PHONY : all clean fclean re