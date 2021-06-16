NAME = minishell
CC = gcc
CFLAGS := -Werror -Wall -Wextra -g
CFLAGS += -fsanitize=address
LINK_LIB := -lreadline
ifeq ($(shell uname), Linux)
	LINK_LIB += -lbsd
endif
ifeq ($(shell uname), Darwin)
	# User have to run brew install readline before run make to build minishell
	CFLAGS += -I $(shell brew --prefix readline)/include
	LINK_LIB += -L$(shell brew --prefix readline)/lib
endif

LIBFT_PATH = libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = -L./libft -lft

LDFLAGS = $(LIBFT_LIB) $(LINK_LIB)

HEADER_FILES = builtin.h env.h lexer.h parse.h	\
	execution.h minishell.h path.h utils.h

SRCS = cmd_cmd_invocation.c cmd_cmd_invocation2.c cmd_exec_command.c		\
	cmd_exec_commands.c cmd_pipe.c cmd_redirection.c convert_ast2cmdinvo.c	\
	cmd_status.c signal.c env_setter.c minishell_error_msg.c env.c exec.c	\
	cmd_exec_builtin.c														\
																			\
	lexer1.c lexer2.c lexer3.c lexer4.c parse1.c parse2.c parse3.c			\
	parse_utils.c parse_utils2.c											\
																			\
	path.c g_cwd.c string_node2string.c expand_env_var.c					\
	expand_string_node.c split_expanded_str.c t_var.c t_var2.c				\
																			\
	builtin.c builtin_echo.c builtin_env.c builtin_exit.c builtin_cd.c		\
	builtin_cd_path.c builtin_cd_chdir.c builtin_cd_cdpath.c				\
	builtin_export.c builtin_export2.c builtin_pwd.c builtin_unset.c		\
																			\
	str_utils.c shell_initialization.c minishell.c	interactive_shell.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(OBJS): $(HEADER_FILES)

$(NAME): ${HEADER_FILES} ${OBJS}
	$(LIBFT_MAKE)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

clean:
	$(LIBFT_MAKE) clean
	${RM} ${OBJS}

fclean: clean
	$(LIBFT_MAKE) fclean
	${RM} ${NAME}

re: fclean all

.PHONY: all clean fclean re
