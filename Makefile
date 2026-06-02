NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -I$(INC_DIR) -MMD -MP

SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = includes

SRC_FILES   = main.c init.c parsing.c
SRCS        = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))
DEPS        = $(OBJS:.o=.d)

RM          = rm -rf

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[32m✓ $(NAME) created.\033[0m"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "\033[34m▶ Compiling $<...\033[0m"
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	@$(RM) $(OBJ_DIR)
	@echo "\033[33m✕ Objects removed.\033[0m"

fclean: clean
	@$(RM) $(NAME)
	@echo "\033[31m✕ Binary removed.\033[0m"

re: fclean all

.PHONY: all clean fclean re
