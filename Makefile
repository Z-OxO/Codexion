CFLAGS = -Wall -Werror -Wextra -lpthread -I includes -ggdb
NAME = codexion


$(NAME): src/main.c src/parsing.c src/init.c
	$(CC) $(CFLAGS) $^ -o $@


all: $(NAME)
