NAME = minishell
CC = gcc
HEADER_FILES = minishell.h
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ./libft/libft.a
SRCS = minishell.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): ${HEADER_FILES} ${OBJS}
	$(CC) -g -fsanitize=address -o $(NAME) $(SRCS) $(LIBFT_LIB) -lbsd

clean:
	$(MLX_MAKE) clean
	$(LIBFT_MAKE) clean
	${RM} ${OBJS}

fclean: clean
	$(LIBFT_MAKE) fclean
	${RM} ${NAME}

re: fclean all

run: all
	./$(NAME)

.PHONY: all clean fclean re run update
