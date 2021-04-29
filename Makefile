NAME = minishell
CC = gcc

LIBFT_PATH = libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ./libft/libft.a

HEADER_FILES = minishell.h
SRCS = cmd_cmd_invocation.c cmd_cmd_invocation2.c cmd_exec_command.c	\
	cmd_exec_commands.c cmd_pid.c cmd_pipe.c convert_ast2cmdinvo.c		\
	env.c exec.c lexer1.c lexer2.c minishell.c parse1.c parse2.c		\
	parse_utils.c parse_utils2.c path.c \
	string_node2string.c expand_env_var.c expand_string_node.c split_expanded_str.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): ${HEADER_FILES} ${OBJS}
	$(LIBFT_MAKE)
	$(CC) -g -fsanitize=address -o $(NAME) $(OBJS) $(LIBFT_LIB) -lbsd

clean:
	$(LIBFT_MAKE) clean
	${RM} ${OBJS}

fclean: clean
	$(LIBFT_MAKE) fclean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
