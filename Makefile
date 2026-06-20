NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread -I$(INC_DIR) -MMD -MP

SRC_DIR     = src
OBJ_DIR     = obj
INC_DIR     = includes

SRC_FILES   = main.c init.c parsing.c utils.c scheduler.c coder.c heap.c monitor.c
SRCS        = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))
DEPS        = $(OBJS:.o=.d)

RM          = rm -rf

GREEN       = \033[32m
BLUE        = \033[34m
YELLOW      = \033[33m
RED         = \033[31m
RESET       = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) created.$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "$(BLUE)▶ Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

check: $(NAME)
	valgrind --tool=helgrind ./$(NAME) 2 200 20 20 20 20 20 fifo

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(YELLOW)✕ Objects removed.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)✕ Binary removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re
