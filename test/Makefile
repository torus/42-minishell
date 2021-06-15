LIBFT_PATH = ../libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ../libft/libft.a
LDFLAGS = $(LIBFT_LIB) -lreadline
SRCFILES = cmd_cmd_invocation.c cmd_cmd_invocation2.c cmd_exec_command.c	\
	cmd_exec_commands.c cmd_pipe.c cmd_redirection.c convert_ast2cmdinvo.c	\
	cmd_status.c signal.c env_setter.c minishell_error_msg.c env.c exec.c	\
	cmd_exec_builtin.c														\
																			\
	lexer1.c lexer2.c lexer3.c parse1.c parse2.c parse_utils.c				\
	parse_utils2.c															\
																			\
	path.c g_cwd.c string_node2string.c expand_env_var.c					\
	expand_string_node.c split_expanded_str.c t_var.c t_var2.c				\
																			\
	builtin.c builtin_echo.c builtin_env.c builtin_exit.c builtin_cd.c		\
	builtin_cd_path.c builtin_cd_chdir.c builtin_cd_cdpath.c				\
	builtin_export.c builtin_export2.c builtin_pwd.c builtin_unset.c		\
																			\
	str_utils.c shell_initialization.c										\
																			\
	rope1.c rope2.c rope3.c rope4.c splay1.c splay2.c splay3.c splay4.c		\
	splay5.c splay6.c

SRCS = $(patsubst %,../%,$(SRCFILES))

HEADERS = ../env.h \
		  ../parse.h \
		  ../execution.h \
		  ../minishell.h
TEST_UTILS = test.c \
			 test.h

VALGRIND = valgrind -q --leak-check=full --errors-for-leak-kinds=all --show-leak-kinds=all --error-exitcode=99

run: parse_test env_test path_test exec_test command_exec_test ast2cmdinvo_test splay_test rope_test
	${VALGRIND} ./parse_test
	${VALGRIND} ./env_test
	${VALGRIND} ./path_test
	${VALGRIND} ./exec_test
	${VALGRIND} ./command_exec_test
	${VALGRIND} ./ast2cmdinvo_test
	${VALGRIND} ./splay_test
	${VALGRIND} ./rope_test

SRCS_PARSE = $(wildcard ../lexer*.c ../parse*.c) ../libft/ft_memcpy.c
SRCS_SPLAY = $(wildcard ../splay*.c)
SRCS_ROPE = $(SRCS_SPLAY) $(wildcard ../rope*.c)	\
	../libft/ft_strlcpy.c ../libft/ft_strlen.c

parse_test: Makefile parse_test.c test.c test.h $(SRCS_PARSE) ../parse.h
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ parse_test.c test.c $(SRCS_PARSE) ${LIBFT_LIB} $(LDFLAGS)

splay_test: Makefile splay_test.c test.c test.h $(SRCS_SPLAY) ../rope.h
	gcc -Werror -Wall -Wextra -g -o $@ splay_test.c test.c $(SRCS_SPLAY) $(LDFLAGS)

rope_test: Makefile rope_test.c test.c test.h $(SRCS_ROPE) ../rope.h
	gcc -Werror -Wall -Wextra -g -o $@ rope_test.c test.c $(SRCS_ROPE) $(LDFLAGS)

env_test: env_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB} $(LDFLAGS)

path_test: path_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB} $(LDFLAGS)

exec_test: exec_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB} $(LDFLAGS)

command_exec_test: command_exec_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB} $(LDFLAGS)

ast2cmdinvo_test: ast2cmdinvo_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB} $(LDFLAGS)

norm:
	docker-compose run --rm norm norminette ../*.[ch]

fclean:
	rm -f ./parse_test
	rm -f ./env_test
	rm -f ./path_test
	rm -f ./exec_test
	rm -f ./command_exec_test
	rm -f ./ast2cmdinvo_test
	rm -f ./rope_test
	rm -f ./splay_test

re: fclean run
