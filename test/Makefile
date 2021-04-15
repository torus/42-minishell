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
	   ../command_exec.c
HEADERS = ../env.h \
		  ../parse.h \
		  ../execution.h
TEST_UTILS = test.c \
			 test.h

run: parse_test env_test path_test exec_test command_exec_test
	./parse_test
	./env_test
	./path_test
	./exec_test
	./command_exec_test

SRCS_PARSE = $(wildcard ../lexer*.c ../parse*.c) ../libft/ft_memcpy.c

parse_test: Makefile parse_test.c test.c test.h $(SRCS_PARSE) ../parse.h
	gcc -Werror -Wall -Wextra -g -o $@ parse_test.c test.c $(SRCS_PARSE)

env_test: env_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

path_test: path_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

exec_test: exec_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

command_exec_test: command_exec_test.c ${TEST_UTILS} ${SRCS} ${HEADERS}
	$(LIBFT_MAKE)
	gcc -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}

norm:
	docker-compose run --rm norm norminette ../*.[ch]
