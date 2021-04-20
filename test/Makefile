LIBFT_PATH = ../libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ../libft/libft.a
SRCS = ../env.c \
	   ../lexer1.c \
	   ../lexer2.c \
	   ../parse1.c \
	   ../parse2.c \
	   ../parse_utils.c \
	   ../exec.c \
	   ../path.c \
	   ../cmd_exec_command.c \
	   ../cmd_exec_commands.c \
	   ../cmd_pipe.c \
	   ../cmd_pid.c \
	   ../cmd_cmd_invocation.c \
	   ../convert_ast2cmdinvo.c
HEADERS = ../env.h \
		  ../parse.h \
		  ../execution.h \
		  ../minishell.h
TEST_UTILS = test.c \
			 test.h

run: parse_test env_test path_test exec_test command_exec_test ast2cmdinvo_test
	./parse_test
	./env_test
	./path_test
	./exec_test
	./command_exec_test
	./ast2cmdinvo_test

SRCS_PARSE = $(wildcard ../lexer*.c ../parse*.c) ../libft/ft_memcpy.c

parse_test: Makefile parse_test.c test.c test.h $(SRCS_PARSE) ../parse.h
	gcc -Werror -Wall -Wextra -g -o $@ parse_test.c test.c $(SRCS_PARSE)

env_test: env_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

path_test: path_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

exec_test: exec_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

command_exec_test: command_exec_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

ast2cmdinvo_test: ast2cmdinvo_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Werror -Wall -Wextra -g -fsanitize=address -o $@ $^ ${LIBFT_LIB}

norm:
	docker-compose run --rm norm norminette ../*.[ch]

fclean:
	rm ./parse_test
	rm ./env_test
	rm ./path_test
	rm ./exec_test
	rm ./command_exec_test
	rm ./ast2cmdinvo_test
