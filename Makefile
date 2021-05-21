NAME = minishell
CC = gcc
CFLAGS = -Werror -Wall -Wextra -g
ifeq ($(shell uname), Linux)
	LINK_LIB = -lbsd
endif

LIBFT_PATH = libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ./libft/libft.a

HEADER_FILES = minishell.h
SRCS = cmd_cmd_invocation.c cmd_cmd_invocation2.c cmd_exec_command.c		\
	cmd_exec_commands.c cmd_pipe.c cmd_redirection.c convert_ast2cmdinvo.c	\
	env.c exec.c lexer1.c lexer2.c lexer3.c minishell.c parse1.c parse2.c	\
	parse_utils.c parse_utils2.c path.c g_cwd.c string_node2string.c		\
	expand_env_var.c expand_string_node.c split_expanded_str.c				\
	cmd_status.c signal.c env_setter.c minishell_error_msg.c				\
	builtin.c builtin_echo.c builtin_env.c builtin_exit.c					\
	builtin_cd.c builtin_cd_path.c builtin_cd_chdir.c builtin_cd_cdpath.c	\
	builtin_export.c builtin_pwd.c builtin_unset.c							\
	str_utils.c shell_initialization.c cmd_exec_builtin.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): ${HEADER_FILES} ${OBJS}
	$(LIBFT_MAKE)
	$(CC) -g -fsanitize=address -o $(NAME) $(OBJS) $(LIBFT_LIB) $(LINK_LIB)

clean:
	$(LIBFT_MAKE) clean
	${RM} ${OBJS}

fclean: clean
	$(LIBFT_MAKE) fclean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
