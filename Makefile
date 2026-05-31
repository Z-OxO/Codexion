CFLAGS = -Wall -Werror -Wextra -lpthread -I includes -ggdb
NAME = codexion


$(NAME): src/main.c src/parsing.c
	$(CC) $(CFLAGS) $^ -o $@


all: $(NAME)
