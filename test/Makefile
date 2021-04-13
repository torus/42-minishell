LIBFT_PATH = ../libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ../libft/libft.a
SRCS = ../env.c \
	   ../parse.c \
	   ../parse2.c \
	   ../exec.c \
	   ../path.c \
	   ../command_exec.c \
	   ../command_exec_spawn_child.c
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

parse_test: Makefile parse_test.c test.c test.h ../parse.c ../parse2.c ../parse.h
	gcc -Wall -Wextra -g -o $@ parse_test.c test.c ../parse.c ../parse2.c

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

clean:
	rm ./parse_test
	rm ./env_test
	rm ./path_test
	rm ./exec_test
	rm ./command_exec_test
