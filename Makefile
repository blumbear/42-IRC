NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes

BUILTINS = 

MAIN = main 

EXECUTING = 

ERROR_MANAGEMENT = errorPrinter

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

all: $(NAME)

clean :
	rm -rf $(OBJ_DIR)

fclean : clean
	rm -rf $(NAME)

re: fclean all

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) -o $@ $(OBJS)

$(OBJ_DIR):
	echo "here"
	mkdir $@

obj/%.o: src/*/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean fclean re